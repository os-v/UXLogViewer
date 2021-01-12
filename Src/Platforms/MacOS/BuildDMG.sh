#
#  @file BuildDMG.sh
#  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
#  @see https://github.com/os-v/UXLogViewer/
#
#  Created on 05.01.21.
#  Copyright 2021 Sergii Oryshchenko. All rights reserved.
#

CurDir=$(cd "$(dirname "$0")"; pwd)

BasePath=../../../Bin
OutputPath=$CurDir/UXLogViewer
AppName=UXLogViewer.app
DMGName=UXLogViewer
DMGFile=UXLogViewer.dmg
CertName="Developer ID Application: "

rm -f -R $OutputPath
rm -f -R "$OutputPath/$AppName"
mkdir $OutputPath

rm ./$DMGFile

cp -a "$BasePath/$AppName" "$OutputPath/$AppName"

codesign --force --verify --verbose --sign "$CertName" "$OutputPath/$AppName"

if [ "$1" == "-w" ]; then
   hdiutil create -size 100m -format UDRW $DMGFile -volname "$DMGName" -fs HFS+ -srcfolder "$OutputPath"
else
   hdiutil create $DMGFile -volname "$DMGName" -fs HFS+ -srcfolder "$OutputPath"
fi

codesign --force --verify --verbose --sign "$CertName" ./$DMGFile
codesign -vv --deep-verify ./$DMGFile
spctl --verbose=4 --assess --type execute -v ./$DMGFile

rm -f -R $OutputPath
mv ./$DMGFile $BasePath
zip $BasePath/Installer.zip $BasePath/$DMGFile


