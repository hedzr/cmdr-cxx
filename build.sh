#!/usr/bin/env bash

VERSION=$(cat .version.cmake|awk '{print substr($2,1,length($2)-1)}')

rebuild(){
	[[ -d ./build ]] && rm -rf ./build
	
	cmake -DENABLE_AUTOMATE_TESTS=OFF -G "Ninja" -S . -B build/
	cmake --build build/
}

build(){
	cmake --build build/
}

pack(){
	[[ -f build/CPackConfig.cmake ]] && {
		cpack --config build/CPackConfig.cmake
		is_debian_series && dpkg --info dist/libcmdr11-dev_$VERSION_$(i386_amd64).deb
		is_redhat_series && rpm -qip dist/cmdr11-$VERSION-1.$(x86_64).rpm
	}
}

version(){
	echo $VERSION
}



osid(){      # fedora / ubuntu
	[[ -f /etc/os-release ]] && {
		grep -Eo '^ID="?(.+)"?' /etc/os-release|sed -r -e 's/^ID="?(.+)"?/\1/'
	}
}
is_fedora(){ [[ "$osid" == fedora ]]; }
is_centos(){ [[ "$osid" == centos ]]; }
is_redhat(){ [[ "$osid" == redhat ]]; }
is_debian(){ [[ "$osid" == debian ]]; }
is_ubuntu(){ [[ "$osid" == ubuntu ]]; }
is_debian(){ [[ "$osid" == debian ]]; }
is_yum   (){ which yum 2>/dev/null; }
is_dnf   (){ which dnf 2>/dev/null; }
is_apt   (){ which apt 2>/dev/null; }
is_redhat_series(){ is_yum || is_dnf; }
is_debian_series(){ is_apt; }
i386_amd64(){ dpkg --print-architecture; }
x86_64(){ uname -m; }


$*