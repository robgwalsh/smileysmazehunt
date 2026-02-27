; Inno Setup Script for Smiley's Maze Hunt
; Compile with Inno Setup 6 (https://jrsoftware.org/isinfo.php)

#define AppName "Smiley's Maze Hunt"
#define AppVersion "3"
#define AppExe "SmileysMazeHunt3.exe"

[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
OutputDir=.
OutputBaseFilename=SmileysMazeHuntSetup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
SetupIconFile=icon1.ico
UninstallDisplayName={#AppName}
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
DisableProgramGroupPage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"

[Files]
; Root executables and DLLs
Source: "SmileysMazeHunt3.exe"; DestDir: "{app}";          Flags: ignoreversion
Source: "bass.dll";              DestDir: "{app}";          Flags: ignoreversion
Source: "hge.dll";               DestDir: "{app}";          Flags: ignoreversion

; Data files
Source: "Data\Enemies.dat";        DestDir: "{app}\Data";              Flags: ignoreversion
Source: "Data\Fonts.zip";          DestDir: "{app}\Data";              Flags: ignoreversion
Source: "Data\GameData.zip";       DestDir: "{app}\Data";              Flags: ignoreversion
Source: "Data\Sounds.zip";       DestDir: "{app}\Data";              Flags: ignoreversion
Source: "Data\GameText.dat";       DestDir: "{app}\Data";              Flags: ignoreversion
Source: "Data\Smiley.ini";         DestDir: "{app}\Data";              Flags: ignoreversion
Source: "Data\Resources.r";     DestDir: "{app}\Data";              Flags: ignoreversion

; Map files
Source: "Data\Maps\castle.smh";    DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\conserve.smh";  DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\debug.smh";     DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\despair.smh";   DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\forest.smh";    DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\fountain.smh";  DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\oldetowne.smh"; DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\path.smh";      DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\smhollow.smh";  DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\snow.smh";      DestDir: "{app}\Data\Maps"; Flags: ignoreversion
Source: "Data\Maps\tutstomb.smh";  DestDir: "{app}\Data\Maps"; Flags: ignoreversion

; Graphics (includes ending/ subfolder automatically via recursesubdirs)
Source: "Graphics\*"; DestDir: "{app}\Graphics"; Flags: ignoreversion recursesubdirs createallsubdirs

[Dirs]
; Create empty Save directory that the game writes to at runtime
Name: "{app}\Data\Save"

[Icons]
Name: "{group}\{#AppName}";                          Filename: "{app}\{#AppExe}"
Name: "{group}\Uninstall {#AppName}";                Filename: "{uninstallexe}"
Name: "{userdesktop}\{#AppName}";                  Filename: "{app}\{#AppExe}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExe}"; Description: "Launch {#AppName}"; Flags: nowait postinstall skipifsilent
