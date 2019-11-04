[Setup]
;SignTool=signtool
AppName=CoralPlus
AppVerName=CoralPlusPack
DefaultGroupName=CoralPlus
UninstallDisplayIcon="C:\Program Files (x86)\CoralPlus\"
SourceDir="C:\work\Project\Qt\CoralPlus\data\win"
OutputDir="out"
OutputBaseFilename=CoralPlusInst
AppPublisher=SalaraSoft
AppPublisherURL=https://github.com/salara1960/CoralPlus

CreateAppDir=yes
DisableDirPage=no
DefaultDirName={code:GetDP}


[Types]
Name: "all"; Description: "all"; Flags: iscustom

[Dirs]
Name: {app}; Permissions: everyone-modify

[Components]
Name: "base"; Description: "Client part"; Types: all; Flags: fixed
Name: "srv"; Description: "Server part"; Types: all;

[Files]
Source: "CoralPlusSrv.exe"; DestDir: "{app}"; Components: srv
Source: "CoralPlusCli.exe"; DestDir: "{app}"; Components: base;

Source: "demo.dmp"; DestDir: "{app}"; Components: srv
Source: "coral.ip"; DestDir: "{app}"; Components: srv
Source: "coral.npl"; DestDir: "{app}"; Components: base
Source: "coral.trk"; DestDir: "{app}"; Components: base
Source: "coral.pdf"; DestDir: "{app}"; Components: base
Source: "key.key"; DestDir: "{app}"; Components: srv
Source: "key.pem"; DestDir: "{app}"; Components: srv

Source: "libs\*.dll"; DestDir: "{app}"; Components: base
Source: "png\*.*"; DestDir: "{app}\png\"; Components: base

Source: "delete.ico"; DestDir: "{app}"; Components: base;
Source: "srv_main.ico"; DestDir: "{app}"; Components: srv; 
Source: "cli_main.ico"; DestDir: "{app}"; Components: base; 


[Icons]
Name: "{group}\CP delete"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; IconFilename: "{app}\delete.ico"

Name: "{group}\CP Server"; Filename: "{app}\CoralPlusSrv.exe"; Parameters: "mode=demo -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\srv_main.ico";
Name: "{commonstartup}\CP Server"; Filename: "{app}\CoralPlusSrv.exe"; Parameters: "mode=demo -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\srv_main.ico";
Name: "{group}\CP Client"; Filename: "{app}\CoralPlusCli.exe"; Parameters: "serverip=127.0.0.1 cfgfile=coral.npl -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\cli_main.ico";
Name: "{commonstartup}\CP Client"; Filename: "{app}\CoralPlusCli.exe"; Parameters: "serverip=127.0.0.1 cfgfile=coral.npl -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\cli_main.ico";


[Code]

const
  DefSelPoint = 'irgtest';
//--------------------------------------------------------------
var
  dd : string;
  DataDirPage: TInputDirWizardPage;
  Sellpoint: TInputQueryWizardPage;

//--------------------------------------------------------------
function PrepareToInstall(var NeedsRestart: Boolean): String;
var
rc:integer;
begin
    result := '';
end;
//--------------------------------------------------------------
function addsp(s:string) :String;
begin
  Result:=Sellpoint.Values[0];
end;
//--------------------------------------------------------------
function InitializeSetup(): Boolean;
begin
  Result := MsgBox('Initialize Setup : ' #13#13 'Install CoralPlus packet ?', mbConfirmation, MB_YESNO) = idYes;
  if Result = False then MsgBox('InitializeSetup canceled :' #13#13 'Bye, bye.', mbInformation, MB_OK);
end;
//--------------------------------------------------------------
procedure InitializeWizard;
var
t:string;
begin

end;
//--------------------------------------------------------------
function GetDP(Param: string): string;
begin
    dd:=ExpandConstant('{pf32}')+'\CoralPlus\';
    result:=dd;
end;
//--------------------------------------------------------------
function MyConst(Param: String): String;
begin
  Result := ExpandConstant('{pf}');
end;
//--------------------------------------------------------------
function MyConstSrc(Param: String): String;
begin
  Result := ExpandConstant('{src}');
end;
//--------------------------------------------------------------
function MoveLogfileToLogDir():boolean;
var
  logfilepathname, logfilename, newfilepathname: string;
begin
  logfilepathname := expandconstant('{log}');
  logfilename := ExtractFileName(logfilepathname);
  try
       newfilepathname := expandconstant('{app}\');
  except                                       
          Result := false;
  end;  
  result := ForceDirectories(newfilepathname); //Make sure the destination path exists.
  newfilepathname := newfilepathname + logfilename; //Add filename

  result := filecopy(logfilepathname, newfilepathname, false);

  if result then
     result := DeleteFile(logfilepathname);
end;
//--------------------------------------------------------------
procedure DeinitializeSetup();
begin
  MoveLogfileToLogDir;
end;
//--------------------------------------------------------------
procedure DelMyProg;
var
  si2,si3, st2,st3 : string;
begin
  sleep(1500);
  si2:=ExpandConstant('{app}');         st2:=si2 + '\CoralPlusSrv.exe';
  if not DeleteFile(st2) then MsgBox('DeleteFile:' #13#13 'Delete file error ! ', mbError, MB_OK);
sleep(1500);
      si3:=ExpandConstant('{app}');         st3:=si3 + '\CoralPlusCli.exe';
  if not DeleteFile(st3) then MsgBox('DeleteFile:' #13#13 'Delete file error ! ', mbError, MB_OK);

end;
//--------------------------------------------------------------

