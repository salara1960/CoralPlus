[Setup]
;SignTool=signtool
AppName=CoralPlus
AppVerName=CoralPlusPack
;DefaultDirName="C:\Program Files (x86)\Smart\"
DefaultGroupName=CoralPlus
UninstallDisplayIcon="C:\Program Files (x86)\CoralPlus\"
SourceDir="C:\work\Project\Qt\CoralPlus\data\win"
OutputDir="out"
OutputBaseFilename=CoralPlusInst
AppPublisher=SalaraSoft
AppPublisherURL=https://github.com/salara1960/CoralPlus
;DisableProgramGroupPage=yes
;DisableWelcomePage=yes
;DisableReadyPage=yes
;DisableStartupPrompt=yes
;SetupLogging=yes
;DisableDirPage=yes

; aad 28.04.2018
CreateAppDir=yes
DisableDirPage=no
DefaultDirName={code:GetDP}


[Types]
Name: "all"; Description: "all"; Flags: iscustom

[Dirs]
Name: {app}; Permissions: everyone-modify

[Components]
Name: "base"; Description: "Qt program"; Types: all; Flags: fixed
;Name: "vcr"; Description: "Visual C++ Redistributable 2015"; Types: all;

[Files]
Source: "CoralPlusSrv.exe"; DestDir: "{app}"; Components: base
Source: "CoralPlusCli.exe"; DestDir: "{app}"; Components: base;

;Source: "x86\libeay32.dll"; DestDir: "{app}"; Components: base; Check: not isWin64;
;Source: "x86\ssleay32.dll"; DestDir: "{app}"; Components: base; Check: not isWin64;
;Source: "x64\libeay32.dll"; DestDir: "{app}"; Components: base; Check: isWin64;
;Source: "x64\ssleay32.dll"; DestDir: "{app}"; Components: base; Check: isWin64;

Source: "demo.dmp"; DestDir: "{app}"; Components: base
Source: "coral.ip"; DestDir: "{app}"; Components: base
Source: "coral.npl"; DestDir: "{app}"; Components: base
Source: "coral.trk"; DestDir: "{app}"; Components: base
Source: "coral.pdf"; DestDir: "{app}"; Components: base
Source: "key.key"; DestDir: "{app}"; Components: base
Source: "key.pem"; DestDir: "{app}"; Components: base
;Source: "README.md"; DestDir: "{app}"; Components: base

Source: "libs\*.dll"; DestDir: "{app}"; Components: base
Source: "png\*.*"; DestDir: "{app}\png\"; Components: base
;Source: "imageformats\*.dll"; DestDir: "{app}"; Components: base

;Source: "uninstall-MSP.bat"; DestDir: "{app}"; Components: base; 
Source: "delete.ico"; DestDir: "{app}"; Components: base;
Source: "srv_main.ico"; DestDir: "{app}"; Components: base; 
Source: "cli_main.ico"; DestDir: "{app}"; Components: base; 


;;Source: "vc_redist.x86.exe"; DestDir: "{tmp}"; AfterInstall: AfterInstall()
;Source: "vc_redist.x86.exe"; DestDir: "{tmp}";

; aad 28.04.2018
;[Registry]
;Root: HKCU; SubKey: "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers"; ValueType: string; ValueName: "{code:GetDP}\Smart.exe"; ValueData: "WINXPSP2"; Flags: uninsdeletevalue


[Icons]
Name: "{group}\CP delete"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"; IconFilename: "{app}\delete.ico"

Name: "{group}\CP Server"; Filename: "{app}\CoralPlusSrv.exe"; Parameters: "mode=demo -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\srv_main.ico";
Name: "{commonstartup}\CP Server"; Filename: "{app}\CoralPlusSrv.exe"; Parameters: "mode=demo -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\srv_main.ico";
Name: "{group}\CP Client"; Filename: "{app}\CoralPlusCli.exe"; Parameters: "serverip=127.0.0.1 cfgfile=coral.npl -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\cli_main.ico";
Name: "{commonstartup}\CP Client"; Filename: "{app}\CoralPlusCli.exe"; Parameters: "serverip=127.0.0.1 cfgfile=coral.npl -platformpluginpath {app}"; WorkingDir: "{app}"; IconFilename: "{app}\cli_main.ico";



[INI]
;Filename: "{app}\conf.ini"; Section: "Smart"; Key: "sell_point"; String: "{code:addsp}";

[Code]
const
  DefSelPoint = 'irgtest';
//--------------------------------------------------------------
var
  dd : string;
  DataDirPage: TInputDirWizardPage;
  Sellpoint: TInputQueryWizardPage;

//--------------------------------------------------------------
//procedure BeforeMyProgInstall;
//var
//   name : string;
//begin
//    name := ExpandConstant('{tmp}'); 
//    name := name + '\adb.exe kill-server';
//    if not Exec(ExpandConstant(name), '', '', SW_SHOWNORMAL, ewWaitUntilTerminated, RC) then
//            MsgBox('BeforeMyProgInstall:' #13#13 'The file ' + name + ' could not be executed. ' + SysErrorMessage(RC) + '.', mbError, MB_OK);
//
//end;
//--------------------------------------------------------------
function PrepareToInstall(var NeedsRestart: Boolean): String;
var
rc:integer;
begin
    {if Exec('taskkill.exe', '/f /im adb.exe"', '', SW_SHOW, ewWaitUntilTerminated, rc) then result:=''}
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
{
  DataDirPage := CreateInputDirPage(wpSelectDir,
                                  'Select Personal Data Directory', 
                                  'Where should personal data files be installed?',
                                  'Select the folder in which Setup should install personal data files, then click Next.',
                                  False, '');
  DataDirPage.Add(''); 
  Sellpoint := CreateInputQueryPage(wpselectprogramgroup,
                                  'Указание точки продаж', ' ',
                                  'Укажите наименование точки продаж и нажмите "Next"');
  Sellpoint.Add('Точка продаж:', False);
}
// Sellpoint.Values[0]:= GetPreviousData('IniDef', GetIniString('Smart', 'sell_point', DefSelPoint, ExpandConstant('{src}\conf.ini')));
// DataDirPage.Values[0] := GetPreviousData('Name', ExpandConstant('{pf32}\Smart')); 
{  
  Sellpoint := CreateInputQueryPage(wpselectprogramgroup, 'Указание точки продаж', ' ', 'Укажите наименование точки продаж и нажмите "Next"');
  Sellpoint.Add('Точка продаж:', False);
  t:=ExpandConstant(dd);
  if Pos('/', ParamStr(ParamCount))=0 then Sellpoint.Values[0] := paramstr(ParamCount)
  else 
    if FileExists(t+'\conf.ini') then
      Sellpoint.Values[0]:= GetPreviousData('IniDef', GetIniString('Smart', 'sell_point', 'def value', t+'\conf.ini'))
    else Sellpoint.Values[0] := 'svyaznoiintegration';
}
//    Sellpoint.Values[0] := '';
end;
//--------------------------------------------------------------
function GetDP(Param: string): string;
begin
    dd:=ExpandConstant('{pf32}')+'\CoralPlus\';
    //MsgBox(ExpandConstant(dd), mbInformation, MB_OK);
    result:=dd;//ExpandConstant(DataDirPage.Values[0]);
end;
//--------------------------------------------------------------
//procedure AfterInstall;
//var
//  RC: Integer;
//  name, namep : string;
//begin
//
//  namep := ExpandConstant('{tmp}');
// 
//  if IsComponentSelected('drv') then
//  begin  
//    name := namep + '\Drivers.exe';
//    if not Exec(ExpandConstant(name), '', '', SW_SHOWNORMAL, ewWaitUntilTerminated, RC) then
//            MsgBox('AfterMyProgInstall:' #13#13 'The file ' + name + ' could not be executed. ' + SysErrorMessage(RC) + '.', mbError, MB_OK);
//  end;
//
//  if IsComponentSelected('vcr') then
//  begin  
//    name := namep + '\vc_redist.x86.exe';
//    if not Exec(ExpandConstant(name), '', '', SW_SHOWNORMAL, ewWaitUntilTerminated, RC) then
//            MsgBox('AfterRedistInstall:' #13#13 'The file ' + name + ' could not be executed. ' + SysErrorMessage(RC) + '.', mbError, MB_OK);
//  end;
//
//    RegWriteStringValue(HKCU, 'Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers',ExpandConstant(ExpandConstant(dd)+'Smart.exe'), 'WINXPSP2');
//    RegWriteStringValue(HKLM, 'Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers',ExpandConstant(ExpandConstant(dd)+'Smart.exe'), 'WINXPSP2');
//end;
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
    //Get the install path by first checking for existing installation thereafter by GUI settings
       newfilepathname := expandconstant('{app}\');
  //except
    //This exception is raised if {app} is invalid i.e. if canceled is pressed on the Welcome page
    //    try
    //      newfilepathname := WizardDirValue+'\smart\'; 
  except                                       
          //This exception is raised if WizardDirValue i s invalid i.e. if canceled is pressed on the Mutex check message dialog.
          Result := false;
    //    end;
  end;  
  result := ForceDirectories(newfilepathname); //Make sure the destination path exists.
  newfilepathname := newfilepathname + logfilename; //Add filename

  //if copy successful then delete logfilepathname 
  result := filecopy(logfilepathname, newfilepathname, false);

  if result then
   //result := MsgBox(logfilepathname+' &  '+newfilepathname, mbConfirmation, MB_YESNO) = idYes;
     result := DeleteFile(logfilepathname);
end;
procedure DeinitializeSetup();
begin
  MoveLogfileToLogDir;
end;
//--------------------------------------------------------------
procedure DelMyProg;
var
  si1,si2,si3, st1,st2,st3 : string;
begin

sleep(1500);
  si1:=ExpandConstant('{app}');         st1:=si1 + '\uninstall-MSP.bat';
  if not DeleteFile(st1) then MsgBox('DeleteFile:' #13#13 'Delete file error ! ', mbError, MB_OK);
sleep(1500);
      si2:=ExpandConstant('{app}');         st2:=si2 + '\CoralPlusSrv.exe';
  if not DeleteFile(st2) then MsgBox('DeleteFile:' #13#13 'Delete file error ! ', mbError, MB_OK);
sleep(1500);
      si3:=ExpandConstant('{app}');         st3:=si3 + '\CoralPlusCli.exe';
  if not DeleteFile(st3) then MsgBox('DeleteFile:' #13#13 'Delete file error ! ', mbError, MB_OK);

end;
//--------------------------------------------------------------
[Run]
;Filename: {win}\system32\cmd.exe; Parameters: "/C {tmp}\vc_redist_x86.exe /passive"; Flags: runhidden
;Filename: {win}\system32\cmd.exe; Parameters: "/C {tmp}\Drivers.exe /verysilent"; Flags: runhidden
