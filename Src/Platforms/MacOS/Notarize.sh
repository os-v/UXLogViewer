
CurDir=$(cd "$(dirname "$0")"; pwd)

BasePath=../../../Bin
AppFile=UXLogViewer.app
ZipFile=UXLogViewer.zip
BundleID="com.osv.UXLogViewer"
DevIDUser=""
DevIDPass=""
DevIDProv=""
CertName="Developer ID Application: "

codesign --deep --force --verify --verbose --options runtime --sign "$CertName" "$BasePath/$AppFile"
codesign --verify --verbose=4 "$BasePath/$AppFile"

zip -r "$BasePath/$ZipFile" "$BasePath/$AppFile"

xcrun altool --notarize-app --primary-bundle-id "$BundleID" --username "$DevIDUser" --password "$DevIDPass" --asc-provider "$DevIDProv" --file "$BasePath/$ZipFile"

echo "Enter your request ID:"
read RequestID

#xcrun altool --notarization-history 0 -u "$DevIDUser" -p "$DevIDPass"
#xcrun altool --notarization-info "$RequestID" -u "$DevIDUser" -p "$DevIDPass"

xcrun stapler staple "$BasePath/$AppFile"


