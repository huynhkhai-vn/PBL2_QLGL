PBL2 - QUAN LY BAN DO VA TIM DUONG
=================================

PROJECT CO 2 CACH CHAY:

1. GIAO DIEN DO HOA (KHUYEN NGHI)
   - Cai Raylib cho MSYS2 UCRT64 neu chua co:
     pacman -S mingw-w64-ucrt-x86_64-raylib
   - Mo thu muc goc PBL2 - Copy bang VS Code.
   - Chay trong terminal:
     .\build_gui.bat
   - Giao dien co ban cho phep chon diem di/den, tim duong ngan nhat,
     tim duong nhanh nhat va mo phong taxi di theo tuyen.

2. PHIEN BAN CONSOLE
   - Chay:
     .\build.bat

CAU TRUC CHINH
- Data/: du lieu giao lo va duong.
- include/BLL/: nghiep vu tim tuyen va giao dien repository.
- include/DAL/: doc du lieu ban do tu file text.
- include/DataStructures/: cac lop OOP va cau truc du lieu ban do.
- include/dijkstra.h: thuat toan Dijkstra.
- include/taxi.h: lop mo phong taxi goc.
- include/DataStructures/map/: cac lop ban do, giao lo lien ket va adjacency.
- include/DataStructures/priorityqueue/: hang doi uu tien.
- gui_main.cpp: chuong trinh giao dien Raylib.
- main.cpp: chuong trinh console goc.

LUU Y
- Chay lenh tu thu muc goc PBL2 - Copy de chuong trinh doc duoc Data/.
- build_gui.bat can GCC/MSYS2 UCRT64 va Raylib da duoc cai dat.
- Ma nguon va thuat toan Dijkstra goc duoc giu lai; map duoc bo sung hai ham
  truy cap giao lo/duong theo chi so de ve ban do.
- GUI goi RouteService (BLL); RouteService nhan IMapRepository; TextMapRepository
  (DAL) doc intersections.txt va roads.txt. De thay doi nguon du lieu, hay thay
  implementation repository tai diem khoi tao trong gui_main.cpp.
