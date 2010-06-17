Name: libaccounts-qt
Version: 0.28
Release: 1%{?dist}
Summary: Accounts framework
Group: System Environment/Libraries
License: LGPLv2.1
URL: http://gitorious.org/accounts-sso/accounts-qt
Source0: accounts-qt-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: qt-devel, doxygen, graphviz
BuildRequires: libaccounts-glib-devel >= 0.39
BuildRequires: glib2-devel
BuildRequires: fdupes

%description
Framework to provide accounts

%files
%defattr(-,root,root,-)
%doc README COPYING
%{_libdir}/lib*.so.*
%{_bindir}/account-tool


%package devel
Summary: Development files for accounts-qt
Group: Development/Libraries
Provides: accounts-qt-dev
Requires: %{name} = %{version}-%{release}

%description devel
Headers and static libraries for the accounts.

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_includedir}/*
%{_libdir}/pkgconfig/*
%{_libdir}/qt4/*


%package tests
Summary: Tests for accounts-qt
Group: System Environment/Libraries
Requires: %{name} = %{version}-%{release}

%description tests
Tests for accounts-qt.

%files tests
%defattr(-,root,root,-)
%{_datadir}/%{name}-tests/*
%{_bindir}/accountstest

%package doc
Summary: Documentation for accounts-qt
Group: Documentation

%description doc
Doxygen-generated HTML documentation for the accounts.

%files doc
%defattr(-,root,root,-)
%{_datadir}/doc/*


%prep
%setup -q -n accounts-qt-%{version}

%build
qmake \
    QMF_INSTALL_ROOT=%{_prefix} \
    CONFIG+=release \
    accounts-qt.pro

make %{?_smp_mflags}


%install
rm -Rf %{buildroot}
make INSTALL_ROOT=%{buildroot} install
rm %{buildroot}/%{_datadir}/doc/accounts-qt/html/installdox
%fdupes %{buildroot}/%{_includedir}
%fdupes %{buildroot}/%{_docdir}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -Rf %{buildroot}
