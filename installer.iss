[Setup]
; AppId distinto da quello di Console Strip: i due installer non si sovrascrivono mai
AppId={{6E4B9C52-3A17-4D8E-9F0B-7C21A5D84E36}
AppName=Vocal Strip
AppVersion=1.0.0
AppPublisher=DaniArieta
DefaultDirName={autopf}\Vocal Strip
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=output
OutputBaseFilename=VocalStrip-Setup
Compression=lzma
SolidCompression=yes

[Files]
Source: "dist\Vocal Strip.vst3\*"; DestDir: "{commonpf64}\Common Files\VST3\Vocal Strip.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
