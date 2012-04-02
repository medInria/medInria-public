%define repo free
#define repo nonfree

#Name:           medInria-%{repo}-release
Name:           medInria-release
Version:        2
Release:        1.2
#Summary:        medInria (%{repo}) Repository Configuration
Summary:        medInria Repository Configuration

Group:          System Environment/Base
License:        BSD
URL:            http://med.inria.fr
#Source1:        rpmfusion-%{repo}.repo
#Source2:        rpmfusion-%{repo}-updates.repo
#Source3:        rpmfusion-%{repo}-updates-testing.repo
#Source4:        rpmfusion-%{repo}-rawhide.repo
#Source15:       RPM-GPG-KEY-rpmfusion-%{repo}-fedora-15-primary
#Source16:       RPM-GPG-KEY-rpmfusion-%{repo}-fedora-16-primary
#Source17:       RPM-GPG-KEY-rpmfusion-%{repo}-fedora-17-primary
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:      noarch

Requires:       system-release >= %{version}

# If apt is around, it needs to be a version with repomd support
Conflicts:      apt < 0.5.15lorg3

#%if %{repo} == "nonfree"
#Requires:       rpmfusion-free-release >= %{version}

%description
medInria repository contains medInria and related packages and dependencies 
for rpm/yum based distributions (but currently only Fedora is supported).
#This package contains the medInria GPG key as well as Yum package manager
#configuration files for RPM Fusion's "nonfree" repository, which holds
#software that is not considered as Open Source Software according to the
#Fedora packaging guidelines. 
#%else
#%description
#medInria repository contains medInria and related packages and dependencies 
#for rpm/yum based distributions (but currently only Fedora is supported).
##This package contains the medInria GPG key as well as Yum package manager
##configuration files for RPM Fusion's "free" repository, which holds only
##software that is considered as Open Source Software according to the Fedora
##packaging guidelines. 
#%endif

%prep
echo "Nothing to prep"

%build
echo "Nothing to build"

%install
rm -rf $RPM_BUILD_ROOT

# Create dirs
install -d -m755 \
  $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d
#  $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg  \

# GPG Key
#%{__install} -Dp -m644 \
#    %{SOURCE15} \
#    %{SOURCE16} \
#    %{SOURCE17} \
#    $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg

# compatibility symlink for easy transition to F11
#ln -s $(basename %{SOURCE15}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora

# Links for the keys
#for i in i386 x86_64; do
#  ln -s $(basename %{SOURCE15}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-15-${i}
#  ln -s $(basename %{SOURCE16}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-16-${i}
#  ln -s $(basename %{SOURCE17}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-17-${i}
#  ln -s $(basename %{SOURCE16}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-latest-${i}
#  ln -s $(basename %{SOURCE17}) $RPM_BUILD_ROOT%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-rpmfusion-%{repo}-fedora-rawhide-${i}
#done


# Yum .repo files
#%{__install} -p -m644 %{SOURCE1} %{SOURCE2} %{SOURCE3} %{SOURCE4} \
#    $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d
cat > $RPM_BUILD_ROOT%{_sysconfdir}/yum.repos.d/medInria.repo <<EOF
[medInria]
name=medInria for \$releasever - \$basearch
baseurl=http://med.inria.fr/yum/fedora/\$releasever/\$basearch/
enabled=1
gpgcheck=0
EOF

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
#%{_sysconfdir}/pki/rpm-gpg/*
%config(noreplace) %{_sysconfdir}/yum.repos.d/*

%changelog
* Wed Mar 28 2012 Benoit Bleuze <benoit.bleuze@inria.fr>
> - 1.2 
- Supercedes the unofficial 1.1 release that contained credentials for the beta release
* Wed Jan 18 2012 Theodore Papadopoulo <Theodore.Papadopoulo@inria.fr> - 1.0
- Initial RPM release

