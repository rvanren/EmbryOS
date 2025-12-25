#!/usr/bin/env python3
import os
import sys
import hashlib

# ------------------------------------------------------------
# Helpers
# ------------------------------------------------------------

def file_hash(path):
    """Compute SHA1 hash of a file."""
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()

def count_lines(path):
    """Return number of lines in a text file."""
    with open(path, "r", errors="ignore") as f:
        return sum(1 for _ in f)

def collect_relevant_files(root):
    """
    Return dict mapping relative path -> (linecount, hash)
    for .c/.h files in top level and shared/.
    """
    result = {}
    # top-level
    for f in os.listdir(root):
        if f.endswith((".c")):
            full = os.path.join(root, f)
            result[f] = (count_lines(full), file_hash(full))

    # shared/
    shared_dir = os.path.join(root, "shared")
    if os.path.isdir(shared_dir):
        for f in os.listdir(shared_dir):
            if f.endswith((".c")):
                full = os.path.join(shared_dir, f)
                rel = os.path.join("shared", f)
                result[rel] = (count_lines(full), file_hash(full))
    return result

# ------------------------------------------------------------
# Main logic
# ------------------------------------------------------------

def main():
    if len(sys.argv) != 3:
        print("Usage: ediff.py chapterX chapterY")
        sys.exit(1)

    dir1, dir2 = sys.argv[1], sys.argv[2]

    files1 = collect_relevant_files(dir1)
    files2 = collect_relevant_files(dir2)

    added = sorted(set(files2) - set(files1))
    common = set(files1) & set(files2)
    changed = sorted([f for f in common if files1[f][1] != files2[f][1]])

    lines1 = sum(v[0] for v in files1.values() if v)
    lines2 = sum(v[0] for v in files2.values() if v)
    diff = lines2 - lines1

    print(f"Comparing {dir1} → {dir2}")
    print()

    if added:
        print("Added files:")
        for f in added:
            print("  +", f)
    if changed:
        print("\nChanged files:")
        for f in changed:
            print("  *", f)
    if not added and not changed:
        print("No added or changed files.")
    print()
    print(f"Total .c lines in {dir1}: {lines1}")
    print(f"Total .c lines in {dir2}: {lines2}")
    print(f"Change in .c lines: {diff:+d}")

# ------------------------------------------------------------
if __name__ == "__main__":
    main()
