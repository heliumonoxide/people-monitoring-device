Make sure your VM has set up FTP server with correct firewall configuration. using below command:
sudo apt-get update
sudo apt-get install vsftpd
vsftpd --version

Also, make sure the config of VSFTPD is also correct by using command in debian as below:
sudo cp /etc/vsftpd.conf /etc/vsftpd.conf.bak --> for backup
sudo nano /etc/vsftpd.conf

Edit to:
Listen_ipv6=YES
anonymous_enable=NO
pasv_enable=YES
pasv_min_port=1024
pasv_max_port=1048
local_enable=YES
write_enable=YES
local_umask=022
dirmessage_enable=YES
use_localtime=YES
xferlog_enable=YES
connect_from_port_20=YES
chroot_local_user=YES
allow_writeable_chroot=YES
secure_root_dir=/var/run/vsftpd/empty
pam_service_name=vsftpd
rsa_cert_file=/etc/ssl/certs/ssl-cert-snakeoil.pem
rsa_private_key_file=/etc/ssl/private/ssl-cert-snakeoil.key
ssl_enable=NO

after saving by click ctrl+x and y then click enter. Now restart vsftpd server by running command:
sudo systemctl restart vsftpd
sudo systemctl status vsftpd
