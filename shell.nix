{ pkgs ? import <nixpkgs> {} }:
let
  perlLibs = pkgs.perl.withPackages (p: [
    p.FileFindRule
    p.ConfigSimple
  ]);
in
  pkgs.mkShell {
    nativeBuildInputs = with pkgs.buildPackages; [
      qemu
      bochs
      docker

      perlLibs
    ];
}
