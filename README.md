# OAQServer
- **Server cho trò chơi Ô ăn quan**
- **Môi trường:**
  - Visual Studio 15
  - MySQL
- **Triển khai server:**
  - Server được chạy tại cổng 5500 localhost
  - Tạp bảng o_an_quan trong MySQL 
  
| Tên trường | Dữ liệu |
|--------------|-------|
| id | int (PK,AI) | 
| email | varchar(255) |
| password | varchar(255) | 
| nickname | varchar(255) | 
| point | int | 
  - Sửa lại  cấu hình kết nối với mysql trong file process_database.cpp
  - Cài đặt và kết nối với MySQL bằng MySQL Connector C++
    - Video hướng dẫn: https://www.youtube.com/watch?v=yNniOHn9Xe0&t
