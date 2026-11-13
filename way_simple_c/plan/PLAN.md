# Plan for Improving `way_simple_c` Example

## Objective
Enhance the minimal Wayland client example by improving documentation, providing build helpers, cleaning up the build process, and polishing the Nix flake.

## Tasks (in order)
1. **README enhancements**
   - Add sections for plain‑GCC, Makefile, and Nix usage.
   - Update the build command to drop the unnecessary `-lrt` flag.
   - Fix the typo in the *Extending the example* list.
   - Clarify the license statement to point to a LICENSE file.
2. **Create a Makefile**
   - Provide `all` and `clean` targets using `pkg-config` for flags.
3. **Add a `.gitignore`**
   - Ignore compiled binaries, object files, `compile_commands.json`, Nix store artifacts (`result`), and the flake lock file.
4. **Add an MIT `LICENSE` file**
   - Include the full MIT license text.
5. **Update `flake.nix`**
   - Remove the obsolete `-lrt` flag from the `buildPhase` command.
6. **Improve C source error handling**
   - Check the return value of `wl_registry_add_listener` and abort with an error if it fails.
   - Replace the empty remover with `NULL` or a comment explaining its no‑op status.
7. **Refine code comments**
   - Adjust the top‑level comment to state that the program opens a surface and runs a short event loop (no drawing).
8. **Add compilation‑database hint**
   - Mention in the README how to generate `compile_commands.json` with `bear -- make` (or via Nix).
9. **Optional Nix packaging polish** (if time permits)
   - Expose the package as `wayland-example` and add `apps.x86_64-linux.default` so `nix run .#default` works.

## Expected Outcomes
- A clearer, more beginner‑friendly README covering multiple build workflows.
- A convenient `make` command for quick compilation.
- Proper licensing information and ignored build artefacts.
- A cleaner, more portable Nix build that works on modern glibc.
- Slightly more robust example code with better error handling.
- Improved developer experience (IDE support via compilation database).

## Next Steps
1. Review and approve this plan.
2. Switch to an implementation agent (edit/write) to apply the patches.

*If any task needs adjustment, let me know before proceeding.*