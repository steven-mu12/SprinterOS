set -e

IMAGE="$1"
DISK="$2"

if [ -z "$IMAGE" ] || [ -z "$DISK" ]; then
    echo "usage: sprinterloader.sh <image> <disk>"
    echo "example: sprinterloader.sh kernel/build/sprinterOS.img /dev/disk4"
    echo ""
    echo "available disks:"
    diskutil list | grep -E "^/dev/|external|physical" || true
    exit 1
fi

if [ ! -f "$IMAGE" ]; then
    echo "sprinterloader: no such image: $IMAGE"
    exit 1
fi

RAW="/dev/r$(basename "$DISK")"
SIZE=$(wc -c < "$IMAGE" | tr -d ' ')

echo "image : $IMAGE ($SIZE bytes)"
echo "target: $DISK (writing via $RAW)"
echo ""
diskutil info "$DISK" | grep -E "Device / Media Name|Disk Size|Removable Media|Virtual" || true
echo ""
echo "This ERASES the first $SIZE bytes of $DISK, including its partition table."
printf "Type the disk name again to confirm (%s): " "$DISK"
read -r CONFIRM

if [ "$CONFIRM" != "$DISK" ]; then
    echo "sprinterloader: aborted"
    exit 1
fi

diskutil unmountDisk "$DISK"
sudo dd if="$IMAGE" of="$RAW" bs=512 conv=sync
diskutil eject "$DISK"

echo "sprinterloader: wrote $IMAGE to $DISK"
