
# UXLogViewer - Cross platform open source log viewer.

## Latest release downloads
<p>
	<ul>
		<li><a href="https://github.com/os-v/UXLogViewer/files/5804194/UXLogViewer-1.0.1-Android.zip">Android (zipped APK)</a><br></li>
		<li><a href="https://github.com/os-v/UXLogViewer/files/5804196/UXLogViewer-1.0.1-iOS.zip">iOS (zipped IPA)</a><br></li>
		<li><a href="https://github.com/os-v/UXLogViewer/files/5804474/UXLogViewer-1.0.1-MacOS.zip">OSX</a><br></li>
		<li><a href="https://github.com/os-v/UXLogViewer/files/5806650/UXLogViewer-1.0.1-Windows.zip">Windows</a><br></li>
		<li><a href="https://github.com/os-v/UXLogViewer/files/5804455/UXLogViewer-1.0.1-Ubuntu.zip">Ubuntu</a><br></li>
		<li><a href="https://github.com/os-v/UXLogViewer/files/5804441/UXLogViewer-1.0.1-FreeBSD.zip">FreeBSD</a><br></li>
	</ul>
</p>

## Current Features
- Cross platform (Windows, macOS, Linux, FreeBSD, iOS, Android)
- Large files support ( > 4GB) without RAM dependence
- Expression based filtering (regexp, comparison operators, string search, etc. see below)
- Custom log formats editor
- User log formats synchronization to/from server
- Copy selected message to clipboard/results
- DPI Aware UI (High DPI)

## Required Features
- File changes watcher

## Known issues
- Recent files may not work on Android

## Online account
* For theme's configuration roaming between devices online account should be used:
	* Register: https://lv.os-v.pw/Register.php
	* Login: https://lv.os-v.pw/Login.php
* Account can be public to allow downsync without password, or private to require password for downsync. To change account type to private or public login into your account and click 'Make Private' or 'Make Public'.

## Build Process
* Use QT to build:
	* https://www.qt.io/download

## Filtering expressions
* Regular expression or boolean expression should be used to filter log file. If filtering expression doesn't correspond to the common format described below, then entered expression is treated as a regular expression filter.
* Common format of boolean expression is:
	* FIELDNAME FunctionalOperator VALUE [LogicalOperator Expression ... LogicalOperator Expression]
	* where FIELDNAME is the name of the field defined in the selected log theme
	* where VALUE  can be decimal, hex (starts with 0x), or string (enclosed with " or ')
	* Supported LogicalOperator's:
		```
		|| - logical opeartor or
		&& - logical opeartor and
		```
	* Additionally you can use parentheses () to make compound expressions, and use logical operator ! before parentheses if you need to make logical 'not'. 
	* Supported FunctionalOperator's:
		```
		=# - check if field matches regular expression
		!# - check if field doesn't match regular expression
		=~ - check if field contains value
		!~ - check if field doesn't contain value
		=^ - check if field starts with value
		!^ - check if field doesn't start with value
		=$ - check if field ends with value
		!$ - check if field doesn't ends with value
		== - check if field equals to value
		!= - check if field doesn't equal to value
		>= - check if field is greater than or equal to value
		<= - check if field is less than or equal to value
		> - check if field is greater than value
		< - check if field is less than value
		```
	* Examples:
		```
		regular expression search for strings containing tcp0:
			tcp0
		regular expression search for strings starts with tcp0:
			^tcp0
		regular expression search for strings ends with tcp0:
			tcp0$
		boolean expression search for messages with process name TcpDump or TcpService:
			Proc=="TcpDump"||Proc=="TcpService"
		boolean expression search for messages in specified date or with specified process name and message:
			(Date>="2017.10.23"&&Date<="2017.10.24"||Proc=="TcpDump")&&Msg=~"port:"
		boolean expression search for messages with date before 2017.10.23 and after 2017.10.24:
			!(Date>="2017.10.23"&&Date<="2017.10.24")
		```

## Screenshots
<p align="center">
	<img alt="Android" src="https://user-images.githubusercontent.com/73893487/104474505-a98c7a80-55f0-11eb-938e-dcd7bc1b933f.png" width="360" height="640"/>
	<img alt="iOS" src="https://user-images.githubusercontent.com/73893487/104474516-ad200180-55f0-11eb-832d-863a3abf1702.png" width="360" height="640"/>
	<img alt="MacOS" src="https://user-images.githubusercontent.com/73893487/104474524-ae512e80-55f0-11eb-927d-fb19ae06ca9e.png"/>
	<img alt="Windows" src="https://user-images.githubusercontent.com/73893487/104474534-b01af200-55f0-11eb-991d-2a29f2e34000.png"/>
	<img alt="Ubuntu" src="https://user-images.githubusercontent.com/73893487/104474530-aee9c500-55f0-11eb-8a2a-4b3c7c1c2c33.png"/>
	<img alt="FreeBSD" src="https://user-images.githubusercontent.com/73893487/104474511-abeed480-55f0-11eb-887f-eb572b7eca9e.png"/>
</p>

## License
This software is distributed under the [GPL 3.0](https://github.com/os-v/UXLogViewer/blob/master/LICENSE) license. Please read LICENSE for information on the software availability and distribution.

