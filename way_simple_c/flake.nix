{
  description = "Wayland + xkbcommon dev shell";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in
    {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          clang-tools # provides clangd
          gcc
          libxkbcommon
          pkg-config
          wayland # provides wayland-client.h and libraries
          bear # compilation database generator
        ];
      };
      packages.x86_64-linux.default = pkgs.stdenv.mkDerivation {
        pname = "wayland-example";
        version = "0.1";
        src = ./.;
        nativeBuildInputs = [ pkgs.pkg-config ];
        buildInputs = [
          pkgs.wayland
          pkgs.libxkbcommon
        ];
        # Let the default C compiler (`cc`) build the program.
        buildPhase = ''
          cc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -lrt -lm
        '';
        installPhase = ''
          mkdir -p $out/bin 
          cp wayland_app $out/bin/
          '';
      };
    };
}
