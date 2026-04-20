# PES-VCS Lab Report

Sai Harshith <PES1UG24CS398>

See `report.pdf` for the screenshot-rendered submission report.

## Analysis Answers

### Q5.1
To implement pes checkout <branch>, PES would first verify that .pes/refs/heads/<branch> exists, read the commit hash stored there, update .pes/HEAD to contain ref: refs/heads/<branch>, then replace the working directory contents with the tree pointed to by that commit. That means reading the commit object, walking its root tree and subtrees, materializing blob contents as files, creating directories, setting modes, and updating the index to match the checked-out snapshot. The complexity is that checkout is not just changing one ref file: it must safely transform live user files without deleting uncommitted work, must handle files that become directories or directories that become files, and must leave the index, HEAD, and working directory consistent if an error happens halfway.

### Q5.2
A dirty checkout conflict can be detected by comparing the working directory against the index before replacing files. For each tracked index entry, stat the path and compare size and mtime to the index metadata; for higher confidence, re-hash the working file as a blob and compare the blob hash to the index hash. Then read the target branch commit tree and check whether that path would be different in the target snapshot. If the working file differs from the index and the target branch also wants to write a different blob at that path, checkout must refuse because it would overwrite uncommitted work. Deleted tracked files and type changes should be treated the same way.

### Q5.3
Detached HEAD means .pes/HEAD stores a commit hash directly instead of a symbolic ref such as ref: refs/heads/main. New commits made in this state can still be valid: each new commit points to the detached commit as its parent, and HEAD moves to the new hash. The risk is that no branch name points to those commits, so switching away can make them unreachable by normal branch traversal. A user could recover them by creating a branch at the detached commit hash before leaving, by using the printed commit hash from pes commit or pes log, or, in a fuller system, by consulting a reflog and writing that hash into .pes/refs/heads/recovered.

### Q6.1
A mark-and-sweep collector would first enumerate all branch refs in .pes/refs/heads and push their commit hashes onto a stack or queue. It would maintain a hash set of reachable object IDs. For each commit popped, mark it, parse it, enqueue its parent commit if present, and enqueue its root tree. For each tree, mark it, parse every entry, enqueue child trees, and mark blob hashes. After marking, walk .pes/objects and delete every object hash not present in the reachable set. A hash set gives O(1) average membership checks. With 100,000 commits and 50 branches, the commit visit count is at most 100,000 if histories share ancestry and the set avoids revisits. The total object visits are commits plus all distinct reachable trees and blobs; in a snapshot-based repository this may be many more than 100,000, but each unique object is still visited once.

### Q6.2
Concurrent garbage collection is dangerous because commit creation publishes objects in several steps. A process might write new blob and tree objects, but not yet write or update the final commit/ref. A concurrent GC that starts from current branch refs cannot see those new objects, classifies them as unreachable, and deletes one just before the commit object starts referencing it. The result is a commit whose tree or blob is missing. Real Git reduces this risk with conservative pruning rules, lock files, temp files, grace periods for recent unreachable objects, pack/ref locking coordination, and by avoiding deletion of objects that may have been created recently by another process.
