# Plan for Improving `way_simple_c` Example

**Plan to address the README issues and improve the GCC command**

| # | Goal | Action | Neovim command / shell step |
|---|------|--------|-----------------------------|
| 1 | Fix the title underline after the new TOC block | The underline must match the length of the title text (`Wayland minimal client example`). Re‑insert the correct underline (30 `=` characters). | `:%s/^ Wayland minimal client example$/Wayland minimal client example/`<br>`:put =repeat('=',len('Wayland minimal client example'))`<br>`dd` *(delete the old underline line)* |
| 2 | Ensure all TOC links point to existing headings | Verify that every anchor (`#prerequisites`, `#plain-gcc`, etc.) matches a heading in the file (case‑insensitive, spaces → hyphens). Add missing headings or adjust the TOC entries. | `:/## Prerequisites` → confirm exists; repeat for each section. |
| 3 | Add a short tagline / version badge (optional) | Place a badge line after the title, e.g., `![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)`. | `:2put ='![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)'` |
| 4 | Update the GCC build command with stronger warning flags | Replace the current command on line 26 with `gcc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -Wall -Wextra -lm`. | `:26s/.*/gcc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -Wall -Wextra -lm/` |
| 5 | Add a note about the new flags | Append a line after the GCC block: “*Note: `-Wall -Wextra` are added to enable common warnings.*”. | `:28put ='*Note: `-Wall -Wextra` are added to enable common warnings.*'` |
| 6 | Verify Makefile respects `CFLAGS` / `LDFLAGS` (optional) | Open `Makefile`; ensure it uses `$(CFLAGS)` and `$(LDFLAGS)`. If not, modify to: `CFLAGS += -Wall -Wextra` and `LDFLAGS += -lm`. | `:e Makefile` → edit accordingly. |
| 7 | Update the “Prerequisites” section with other distro hints | Add a short bullet after the existing list: `- For Fedora: `dnf install wayland-devel libxkbcommon-devel gcc pkgconf-pkg-config``. | `:/### Prerequisites` → go to that line, then `:normal o- For Fedora: \`dnf install wayland-devel libxkbcommon-devel gcc pkgconf-pkg-config\`` |
| 8 | Add a note that `bear` must be installed for the compilation database | Insert after line 78: `*Make sure `bear` is installed (`sudo apt-get install bear`).*`. | `:78put ='*Make sure `bear` is installed (`sudo apt-get install bear`).*'` |
| 9 | Run a quick visual check | Reload the file (`:e`) and scroll through to confirm headings line up with TOC entries and the underline is correct. | `:e` |
|10| Commit the changes | Use git to stage and commit the updated README (and Makefile if edited). | ```sh<br>git add README.md Makefile<br>git commit -m "Fix README formatting, add TOC, improve GCC flags"<br>``` |

**Next step**  
Open the README in Neovim (`:e README.md`) and apply steps 1‑5. Let me know if you run into any mismatched anchors or need help editing the Makefile.

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
