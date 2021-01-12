#
#  @file Notarize.sh
#  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
#  @see https://github.com/os-v/UXLogViewer/
#
#  Created on 05.01.21.
#  Copyright 2021 Sergii Oryshchenko. All rights reserved.
#

CurDir=$(cd "$(dirname "$0")"; pwd)

BasePath=../../../Bin
AppFile=UXLogViewer.app
ZipFile=UXLogViewer.zip
BundleID="com.osv.UXLogViewer"
DevIDUser=""
DevIDPass=""
DevIDProv=""
CertName="Developer ID Application: "
RequestTimeout=60s

codesign --deep --force --verify --verbose --options runtime --sign "$CertName" "$BasePath/$AppFile"
codesign --verify --verbose=4 "$BasePath/$AppFile"

zip -r "$BasePath/$ZipFile" "$BasePath/$AppFile"

/usr/bin/ditto -c -k --sequesterRsrc --keepParent "$BasePath/$AppFile" "$BasePath/$ZipFile"

#xcrun altool --notarize-app --primary-bundle-id "$BundleID" --username "$DevIDUser" --password "$DevIDPass" --asc-provider "$DevIDProv" --file "$BasePath/$ZipFile"

echo "Uploading, please wait ..."

UploadResult=$(xcrun altool --notarize-app --primary-bundle-id "$BundleID" --username "$DevIDUser" --password "$DevIDPass" --asc-provider "$DevIDProv" --file "$BasePath/$ZipFile" 2>&1)
UploadReturn=$?

echo "$UploadResult"

if [ $UploadReturn -ne 0 ]; then
	echo "Failed to upload, error: $UploadReturn"
	exit $UploadReturn
fi

#echo "Enter your request ID:"
#read RequestID

#echo "$UploadResult" | awk '{if ($1 == "RequestID:") print $2}'
#echo "$UploadResult" | awk '/RequestID:/ {print $2}'
#echo "$UploadResult" | grep "RequestID" | sed -n "s/RequestID:[[:space:]]*\(.*\)/\1/p"

RequestUUID=$(echo "$UploadResult" | grep "RequestUUID" | sed -n "s/RequestUUID = *\(.*\)/\1/p")

echo "RequestUUID is $RequestUUID"

if [ -z $RequestUUID ]; then
	echo "Invalid RequestUUID"
	exit -1
fi

#xcrun altool --notarization-history 0 -u "$DevIDUser" -p "$DevIDPass"
#xcrun altool --notarization-info "$RequestID" -u "$DevIDUser" -p "$DevIDPass"

for ((iAttempt=1; ; iAttempt++))
do
	echo "Requesting notarization result. Attempt #$iAttempt"
	RequestResult=$(xcrun altool --notarization-info "$RequestUUID" -u "$DevIDUser" -p "$DevIDPass" 2>&1)
	RequestReturn=$?
	Status=$(echo "$RequestResult" | grep "Status:" | sed -n "s/[[:space:]]*Status: *\(.*\)/\1/p")
	StatusCode=$(echo "$RequestResult" | grep "Status Code:" | sed -n "s/[[:space:]]*Status Code: *\(.*\)/\1/p")
	echo "   Status=$Status, Status Code=$StatusCode"
	if [ $RequestReturn -ne 0 ]; then
		echo "$RequestResult"
		echo "Request failed, error: $RequestReturn"
		exit $RequestReturn
	fi
	if [ "$Status" == "invalid" ]; then
		echo "FullMessage: $RequestResult"
		exit -1
	fi
	if [ "$Status" == "success" ] && [ $StatusCode -eq 0 ]; then
		break
	fi
	echo "   Suspending for $RequestTimeout"
	sleep $RequestTimeout
done

echo "Notarization succeeded, stapling ..."

xcrun stapler staple "$BasePath/$AppFile"

echo "Notarization finished"

