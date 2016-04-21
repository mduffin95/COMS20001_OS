#Python command to start the disk controller.

dd of=disk.bin if=/dev/zero count=32768 bs=32
python disk.py --host=127.0.0.1 --port=1235 --file=disk.bin --block-num=32768 --block-len=32
