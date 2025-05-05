sudo systemctl stop hab-img-sender
rm ./pic*.jpeg ./tmp.jpeg
echo 0 > index.txt
echo '' > hab.log
sudo systemctl start hab-img-sender

