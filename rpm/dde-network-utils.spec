Name:           dde-network-utils
Version:        5.4.4
Release:        1
Summary:        Deepin desktop-environment - network utils
License:        GPLv3
URL:            https://github.com/linuxdeepin/dde-network-utils
Source0:        %{name}_%{version}.orig.tar.xz	

BuildRequires:  gcc-c++
#BuildRequires:  pkgconfig(dframeworkdbus) >= 2.0
BuildRequires:  dde-qt-dbus-factory-devel
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  qt5-linguist
BuildRequires:  pkgconfig(gsettings-qt)
BuildRequires:  gtest-devel

%description
Deepin desktop-environment - network utils.

%package devel
Summary:        Development package for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
Header files and libraries for %{name}.

%prep
%setup -q -n %{name}-%{version}
sed -i 's|lrelease|lrelease-qt5|' translate_generation.sh
sed -i 's|/lib$|/%{_lib}|' dde-network-utils/dde-network-utils.pro

%build
%qmake_qt5 PREFIX=%{_prefix}
%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}

%files
%doc README.md
%{_libdir}/libdde-network-utils.so.1*
%{_datadir}/%{name}/

%files devel
%{_includedir}/libddenetworkutils/
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libdde-network-utils.so

%changelog
* Thu Mar 11 2021 uniontech <uoser@uniontech.com> - 5.4.4-1
- Update to 5.4.4

