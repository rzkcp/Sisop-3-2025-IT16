# LAPORAN PRAKTIKUM MODUL-3 #
## Praktikum Sistem Operasi Kelompok IT16 ##

| Nama | NRP       |
|-------|-----------|
| Ahmad Syauqi Reza | 5027241085   |
| Mochammad Atha Tajuddin   | 5027241093  |
| Tiara Fatimah Azzahra   | 5027241090  |
---


### Soal_1 ### 
Dalam soal ini intinya praktikan diminta untuk membuat sistem RPC server-client untuk mengubah text file sehingga bisa dilihat dalam bentuk file jpeg. 
- pertama terdapat text file rahasia pada LINK BERIKUT https://drive.google.com/file/d/15mnXpYUimVP1F5Df7qd_Ahbjor3o1cVw/view?usp=sharing , diperbolehkan untuk download/unzip secara manual.
- kedua praktikan ditugaskan untuk membuat image_server.c disini saya menggunakan nano, dimana program yang dibuat harus berjalan secara daemon  
  di background dan terhubung dengan image_client.c melalui socket RPC menggunakan "./server/image_server &" dan "./client/image_client"
- Ketiga program image_client.c harus bisa terhubung dengan image_server.c dan bisa mengirimkan perintah seperti Decrypt text file yang dimasukkan dengan cara Reverse Text lalu Decode from Hex, Request download 
  dari database server sesuai filename yang dimasukkan, misalnya: 1744401282.jpeg. Dengan catatan Note: diperbolehkan copy/pindah file, gunakan RPC untuk mengirim data.
- keempat Program image_client.c harus disajikan dalam bentuk menu kreatif yang memperbolehkan pengguna untuk memasukkan perintah berkali-kali.
![UBUNTU 24 04 2  Running  - Oracle VirtualBox 08_05_2025 23_39_32](https://github.com/user-attachments/assets/d3b643f4-3adc-4eed-9045-de8fc2571290)
- Kelima praktikan harus bisa mengirimkan text file dan menerima sebuah file jpeg yang dapat dilihat isinya melalui program tersebut.

### Soal_2 ###
Praktikan diminta untuk membuat delivery management system untuk RushGo menggunakan 2 program yang saling berinteraksi menggunakan shared memory dan saling terhubung untuk threading (Agent express) dan interaksi user (regular delivery).

Pertama kita compile terlebih dahulu 2 programnya, yaitu dispatcher (untuk regular delivery dan monitoring) dan delivery_agent (untuk express delivery)

![Screenshot 2025-05-08 235205](https://github.com/user-attachments/assets/849eed6a-34ce-4f01-9a9e-fca030bb6a5a)

lalu kita up data pesanan(file csv) ke shared memory.

![Screenshot 2025-05-08 235026](https://github.com/user-attachments/assets/df4b93b6-b8ef-451d-aaf1-f03fe959ccb7)

Fungsi ./delivery_agent akan menjalankan program express delivery menggunakan thread agent(Agent A, Agent B, Agent C) secara otomatis.

![Screenshot 2025-05-08 235717](https://github.com/user-attachments/assets/d5f9f077-ed43-45fb-be80-98eee1e645b4)

Untuk melihat daftar pesanan dan statusnya bisa menggunakan ./dispatcher -list   .

![Screenshot 2025-05-08 235333](https://github.com/user-attachments/assets/c2a2c117-d586-4f55-b3cb-63e647ed4c8b)

Untuk melihat status pengiriman, bisa dilakukan dengan ./dispatcher -status <nama> 

![Screenshot 2025-05-08 235647](https://github.com/user-attachments/assets/9948d417-24d5-4c14-b2b5-e5cc3b3c6677)

Apabila belum dikirim, maka bisa menggunakan fitur regular delivery melalui user dengan command ./dispatcher -deliver <nama>

![Screenshot 2025-05-08 235425](https://github.com/user-attachments/assets/a2021da0-23e3-46b7-bf44-7867ccd70575)

Semua aktivitas yang terjadi disimpan ke dalam log file seperti berikut

![Screenshot 2025-05-09 000030](https://github.com/user-attachments/assets/f050c2bd-51f9-446c-ab1b-7de79b507df4)

### Soal_3 ###
Dalam soal ini intinya praktikan diminta untuk membuat kode program yang menggunakan konsep socket programming, dengan case problemnya yaitu membuat semacam permainan simple, di mana player dapat terhubung dengan kode program utama yang bertugas listing server ke dalam port, dan kode program inilah yang menjadi inti utamanya.

![Struktur File](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_1_3.png)

### 1. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_2_3.png)
   Program utama(dungeon.c) me-listing pada port 8080.
### 2. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_3_3.png)
   Terdapat handling bilamana kita me-run file atau program player.c maka dalam dungeon.c akan memberikan keterangan client/player terhubung.
### 3. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_4_3.png)
  Program player.c,dalam kode berikut player.c berperan sebagai client atau user, dan dalam program ini
  player tidak menyimpan data-data dari list berikut,melainkan hanya opsi switch case saja.
### 4. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_5_3.png)
  Menu ke-1 yaitu berfungsi untuk memeriksa stats awal dari player, di mana sesuai dengan gambar yang ada diatas.
### 5. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_6_3.png)
   Jikalau inventori kosong,maka akan diberikan keterangan bahwa inventori saat ini sedang kosong.
### 6. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_7_3.png)
   menu no.3,yaitu menu atau opsi untuk player dapat melakukan pembelian senjata sebelum melakukan battle atau pertempuran dengan musuh.
   Dalam menu senjata tersebut player tidak menyimpan data-datanya namun file dari dungeon.c lah yang   
   berperan untuk menyimpannya yang terhubung dengan program atau file shop.c
### 7. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_8_3.png)
   Contoh player berhasil membeli senjata
### 8. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_9_3.png)
   Handling keterangan bahwa senjata terkini sedang digunakan
### 9. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_10_3.png)
   Dalam menu battle pertempuran, kita perlu memastikan bahwsannya bilaman player mengetikkan attack dengan berbagai kombinasi,misal ATtacK,atTAcK dsb.
   tidak akan memengaruhi berjalannya pertempuran.
### 10. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_11_3.png)
    Setelah pertempuran selesai maka player akan mendapatkan gold secara random, dalam kasus ini praktikan diberikan kebebasan untuk melakukan random reward & HP enemy logicnya.
### 11. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_12_3.png)
    Dalam kasus ini,jika player mencoba untuk melakukan pertempuran kembali maka HP musuh akan te-reset kembali,
    dan pertempuran atau battle akan diulangi ke tahap awal tanpa memengaruhi rewardnya yang sudah didapatkan(reward tidak akan 
    ikut te-reset)
### 12. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_13_3.png)
    Konfirmasi keterangan jikalau player keluar dari permainan(keluar dari program player.c)
### 13. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_14_3.png)
    Kode program dari file shop.c berisikan list menu senjata yang tersedia, yang nantinya akan di-import ke dalam file dungeon.


### Soal_4 ###

Dalam soal ini praktikan diminta untuk membuat dua program yang bisa saling berinteraksi melalui shared memory, yaitu hunter dan system. Hunter memiliki sistem leveling, untuk meningkatkan levelnya dibutuhkan exp yang didapat dari battle dengan hunter ataupun menaklukan dungeon.


Pertama kita compile dulu program hunter dan systemnya.

![compile](https://github.com/user-attachments/assets/f42e6c93-96e3-4c6f-a50e-d32bd365295e)

Di dalam menu hunter pertama, ada fitur untuk login dan register hunter.

![menu hunter 1](https://github.com/user-attachments/assets/90e30e56-d980-4a08-949d-081ee5f58230)

Setelah login, akan muncul menu hunter kedua, kita bisa menaklukan dungeon, battle sesama hunter, melihat hunter, dan notifikasi (daftar dungeon yang sesuai kondisi level)

![Screenshot 2025-05-09 001736](https://github.com/user-attachments/assets/9b1d9533-51bf-446b-9b41-250e8d962bb6)

![Screenshot 2025-05-09 002935](https://github.com/user-attachments/assets/aa57744c-5fa8-4069-9eb3-d1dbc9e9af8f)

![Screenshot 2025-05-09 001833](https://github.com/user-attachments/assets/e4d25b26-8a9d-4e4b-b920-87912faa746d)

Setelah exp mencapai 500 lebih, maka level akan bertambah
![Screenshot 2025-05-09 001826](https://github.com/user-attachments/assets/9864dc9e-f19b-47b6-bb3a-4a123e7170c9)

Pada menu system, kita bisa membuat dungeon, menampilkan huter dan dungeon, ban dan unban hunter, serta reset hunter

![Screenshot 2025-05-09 002952](https://github.com/user-attachments/assets/c62b2dd0-8d74-4f24-8160-d305a40e7dd0)

![Screenshot 2025-05-09 001449](https://github.com/user-attachments/assets/24035498-b39d-4d3b-a61c-6013f1386b52)

![Screenshot 2025-05-09 001502](https://github.com/user-attachments/assets/87b39b94-5cf8-4ba7-9d52-33ce276b8e61)

![Screenshot 2025-05-09 001608](https://github.com/user-attachments/assets/047e7236-5d5c-4e0d-b06c-769e31870ef8)

apabila dibanned, maka hunter tidak bisa melakukan aktivitas seperti menaklukan dungeon dan battle 1v1.

![Screenshot 2025-05-09 001629](https://github.com/user-attachments/assets/a784115a-83b4-4c14-af70-f539eb47bf13)

Setelah diunbanned, maka akses hunter kembali seperti semula

![Screenshot 2025-05-09 001649](https://github.com/user-attachments/assets/796efc54-b4da-4162-a29f-5c30325d72dc)





