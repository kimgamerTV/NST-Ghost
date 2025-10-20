Name:           bga
Version:        0.1.0
Release:        1%{?dist}
Summary:        Game analyzer core and tools

License:        MIT
URL:            https://github.com/your-org/bga
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  ninja-build
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  qt6-qtdeclarative-devel

%description
Core library and command-line runner extracting assets from game engines such as RPGM and Unity.

%package core
Summary:        Shared library and headers for BGA analyzers

%description core
Shared library exposing analyzer interfaces plus headers for integration into other Qt applications.

%package runner
Summary:        Headless analyzer runner CLI

Requires:       %{name}-core = %{version}-%{release}

%description runner
Command-line utility wrapping BGA analyzer implementations for engine-specific extraction tasks.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake -G Ninja -DBGA_VERSION=%{version}
%cmake_build

%check
ctest --output-on-failure --test-dir %{_vpath_builddir}

%install
%cmake_install

%files core
%license LICENSE
%{_libdir}/libBGACore.so*
%{_includedir}/core

%files runner
%license LICENSE
%{_bindir}/BGARunner

%changelog
* Thu Oct 17 2024 NST Ghost <you@example.com> - 0.1.0-1
- Initial package
