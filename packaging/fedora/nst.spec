Name:           nst
Version:        0.1.0
Release:        1%{?dist}
Summary:        A tool for translating video games.

License:        MIT
URL:            https://github.com/your-org/nst
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  ninja-build
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  qt6-qtdeclarative-devel

%description
A tool for translating video games.

%prep
%autosetup -n %{name}-%{version}

%build
%cmake -G Ninja -DNST_VERSION=%{version}
%cmake_build

%check
ctest --output-on-failure --test-dir %{_vpath_builddir}

%install
%cmake_install

%files
%license LICENSE
%{_bindir}/NST
%{_datadir}/icons/hicolor/scalable/apps/nst.svg
%{_datadir}/applications/nst.desktop

%changelog
* Thu Oct 17 2024 NST Ghost <you@example.com> - 0.1.0-1
- Initial package
