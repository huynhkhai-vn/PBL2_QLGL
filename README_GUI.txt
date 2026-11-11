PBL2 - GIAO DIEN DO HOA QT 6

1. Cai dat MSYS2 UCRT64 va dam bao g++/pkg-config co trong PATH.
2. Cai Qt 6 Widgets trong terminal MSYS2 UCRT64:
   pacman -S mingw-w64-ucrt-x86_64-qt6-base
3. Mo thu muc goc project bang VS Code.
4. Chay build_gui.bat.

Giao dien:
- Chon vi tri taxi, diem don va diem den bang danh sach hoac bam vai tro
  roi chon giao lo tren ban do.
- Tim duong ngan nhat hoac nhanh nhat theo thuat toan Dijkstra.
- Mo phong taxi di theo tuyen da tim; xoa ket qua de lam lai.
- Quan ly du lieu de them, sua va xoa giao lo/duong; thay doi duoc luu vao
  hai file trong Data/.
- Lan chuot de zoom, giu chuot phai va keo de di chuyen ban do.

Chay tu thu muc goc de chuong trinh doc dung Data/intersections.txt va
Data/roads.txt. Giao dien Qt khong con phu thuoc Raylib.
