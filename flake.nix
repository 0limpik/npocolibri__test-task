rec {
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
    nixpkgs-qt.url = "github:NixOS/nixpkgs/3c584440afe9d4e747c9a5d4942977a5fa05191f";
  };

  outputs =
    {
      self,
      nixpkgs,
      nixpkgs-qt,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      pkgs-qt = import nixpkgs-qt { inherit system; };
      qt6 = pkgs-qt.qt6;
      qtenv =
        with qt6;
        env "qt-custom-${qtbase.version}" [
          qtbase
          wrapQtAppsHook
        ];
      shell = pkgs.mkShell {
        buildInputs = with pkgs; [
          qtenv
          qtcreator

          valgrind
          kdePackages.kcachegrind
          kdePackages.massif-visualizer

          libglvnd
          cmake
          clang
          clang-tools
          python3
          python313Packages.pip

          bashInteractive
        ];

        shellHook = ''
          export QT_PLUGIN_PATH="''$(pwd)/build/Desktop-Debug/:${qtenv}/lib/qt-6/plugins"
          export SHELL="/run/current-system/sw/bin/bash"

          unset QT_STYLE_OVERRIDE
        '';
      };
    in
    {
      packages."${system}".default = shell;
      devShells."${system}".default = shell;
    };
}
