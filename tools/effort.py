"""Measure the effort behind the reconstruction from the Claude Code session
transcripts that did the work, for the Effort section of
progress/index.html: elapsed and active time, agents run, tokens, and what
those tokens would cost at Anthropic's API list prices.

    python3 -I tools/effort.py          refresh progress/effort.json and print it

The sessions are listed by id in private/effort_sessions.txt (one per line;
private/ is not published). Transcripts are read from
~/.claude/projects/*/<id>.jsonl plus everything under <id>/ (subagents and
workflow agents). Each file's sums are cached in build/effort_cache.json by
size and mtime, so a refresh after every pipeline step stays cheap. Without
the list or the transcripts nothing is measured and progress/effort.json
keeps its last figures.
"""
import glob
import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "progress", "effort.json")
CACHE = os.path.join(ROOT, "build", "effort_cache.json")
LIST = os.path.join(ROOT, "private", "effort_sessions.txt")
PROJECTS = os.path.expanduser("~/.claude/projects")
IDLE = 15 * 60  # a pause longer than this between records is not counted as active time
TOKENS = ("input_tokens", "cache_creation_input_tokens", "cache_read_input_tokens", "output_tokens")
# API list prices, US$ per million tokens (Anthropic pricing as of 2026-11-03):
# input, cache write with the 5-minute TTL, cache write with the 1-hour TTL,
# cache read, output. Every request in the transcripts ran at standard speed.
PRICES = {
    "claude-opus-5-5": (4.00, 5.00, 8.00, 0.20, 20.00),
    "claude-fable-5-1": (10.00, 12.50, 20.00, 0.25, 50.00),
}
BUCKETS = ("input", "write_5m", "write_1h", "read", "output")
CACHE_VERSION = 2  # bump when read_file's result changes shape


def stamp(ts):
    """ISO time 2026-10-21T15:31:02.389Z -> seconds (local arithmetic only)."""
    import datetime
    return datetime.datetime.fromisoformat(ts.replace("Z", "+00:00")).timestamp()


def read_file(path):
    """Token sums (each API message once: a streamed message repeats its usage
    on every record) and the sorted record times of one transcript."""
    sums = dict.fromkeys(TOKENS, 0)
    models = {}
    seen, times = set(), []
    for line in open(path, encoding="utf-8", errors="replace"):
        try:
            r = json.loads(line)
        except ValueError:
            continue
        if not isinstance(r, dict):
            continue
        if r.get("timestamp"):
            times.append(stamp(r["timestamp"]))
        m = r.get("message")
        if not isinstance(m, dict) or not isinstance(m.get("usage"), dict):
            continue
        key = (m.get("id"), r.get("requestId"))
        if m.get("id") and key in seen:
            continue
        seen.add(key)
        u = m["usage"]
        for k in TOKENS:
            sums[k] += u.get(k) or 0
        # a write without the TTL split is priced as a 5-minute write
        split = u.get("cache_creation") or {}
        w1h = split.get("ephemeral_1h_input_tokens") or 0
        b = models.setdefault(m.get("model") or "?", dict.fromkeys(BUCKETS, 0))
        b["input"] += u.get("input_tokens") or 0
        b["write_1h"] += w1h
        b["write_5m"] += (u.get("cache_creation_input_tokens") or 0) - w1h
        b["read"] += u.get("cache_read_input_tokens") or 0
        b["output"] += u.get("output_tokens") or 0
    return {"tokens": sums, "models": models, "times": sorted(times)}


def active(times):
    return sum(b - a for a, b in zip(times, times[1:]) if b - a <= IDLE)


def refresh():
    """Re-measure and rewrite progress/effort.json; returns its contents
    (the stored figures when the transcripts are not on this machine)."""
    old = json.load(open(OUT)) if os.path.exists(OUT) else None
    if not os.path.exists(LIST):
        return old
    ids = [l.strip() for l in open(LIST) if l.strip() and not l.startswith("#")]
    mains, agents = [], []
    for sid in ids:
        mains += glob.glob(os.path.join(PROJECTS, "*", sid + ".jsonl"))
        for d in glob.glob(os.path.join(PROJECTS, "*", sid)):
            agents += glob.glob(os.path.join(d, "**", "*.jsonl"), recursive=True)
    if not mains:
        return old
    try:
        cache = json.load(open(CACHE))
    except (OSError, ValueError):
        cache = {}
    per = {}
    for p in mains + agents:
        st = os.stat(p)
        sig = [st.st_size, st.st_mtime, CACHE_VERSION]
        if cache.get(p, {}).get("sig") != sig:
            cache[p] = {"sig": sig, **read_file(p)}
        per[p] = cache[p]
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    json.dump({p: v for p, v in cache.items() if p in per}, open(CACHE, "w"))
    tok = dict.fromkeys(TOKENS, 0)
    for v in per.values():
        for k in TOKENS:
            tok[k] += v["tokens"][k]
    by_model = {}
    for v in per.values():
        for name, b in v["models"].items():
            acc = by_model.setdefault(name, dict.fromkeys(BUCKETS, 0))
            for k in BUCKETS:
                acc[k] += b[k]
    cost, unpriced = 0.0, 0
    for name, b in by_model.items():
        if name in PRICES:
            cost += sum(b[k] * PRICES[name][i] for i, k in enumerate(BUCKETS)) / 1e6
        elif name != "<synthetic>":   # local placeholder records, no API call
            unpriced += sum(b.values())
    main_times = sorted(t for p in mains for t in per[p]["times"])
    agent_active = sum(active(per[p]["times"]) for p in agents)
    import datetime
    iso = lambda s: datetime.datetime.fromtimestamp(s, datetime.timezone.utc).isoformat(timespec="seconds")
    out = {
        "first": iso(main_times[0]), "last": iso(main_times[-1]),
        "elapsed_days": round((main_times[-1] - main_times[0]) / 86400, 1),
        "session_hours": round(active(main_times) / 3600, 1),
        "agent_hours": round(agent_active / 3600, 1),
        "agents": len(agents),
        "tokens": tok,
        "tokens_total": sum(tok.values()),
        "tokens_new": sum(tok.values()) - tok["cache_read_input_tokens"],
        "models": by_model,
        "api_cost_usd": round(cost),
        "unpriced_tokens": unpriced,
    }
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    json.dump(out, open(OUT, "w"), indent=1)
    return out


if __name__ == "__main__":
    e = refresh()
    if e is None:
        sys.exit("effort: no private/effort_sessions.txt or transcripts here; nothing measured")
    print(json.dumps(e, indent=1))
