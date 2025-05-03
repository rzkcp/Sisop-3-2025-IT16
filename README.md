# LAPORAN PRAKTIKUM MODUL-3 #
## Praktikum Sistem Operasi Kelompok IT16 ##

| Nama | NRP       |
|-------|-----------|
| Ahmad Syauqi Reza | 5027241085   |
| Mochammad Atha Tajuddin   | 5027241093  |
| Tiara Fatimah Azzahra   | 5027241090  |
---

### Soal_3 ###
Dalam soal ini intinya praktikan diminta untuk membuat kode program yang menggunakan konsep socket programming, dengan case problemnya yaitu membuat semacam permainan simple, di mana player dapat terhubung dengan kode program utama yang bertugas listing server ke dalam port, dan kode program inilah yang menjadi inti utamanya.

Ketentuan struktur file : 
![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_1_3.png)

1. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_2_3.png)
   Program utama(dungeon.c) me-listing pada port 8080.
2. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_3_3.png)
   Terdapat handling bilamana kita me-run file atau program player.c maka dalam dungeon.c akan memberikan keterangan client/player terhubung.

  #Program player.c#
3. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_4_3.png)
  Dalam kode berikut player.c berperan sebagai client atau user, dan dalam program ini
  player tidak menyimpan data-data dari list berikut,melainkan hanya opsi switch case saja.
4. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_5_3.png)
  Menu ke-1 yaitu berfungsi untuk memeriksa stats awal dari player, di mana sesuai dengan gambar yang ada diatas.

5. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_6_3.png)
   Jikalau inventori kosong,maka akan diberikan keterangan bahwa inventori saat ini sedang kosong.

6. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_7_3.png)
   menu no.3,yaitu menu atau opsi untuk player dapat melakukan pembelian senjata sebelum melakukan battle atau pertempuran dengan musuh.
   Dalam menu senjata tersebut player tidak menyimpan data-datanya namun file dari dungeon.c lah yang   
   berperan untuk menyimpannya yang terhubung dengan program atau file shop.c

7. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_8_3.png)
   Contoh player berhasil membeli senjata

8. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_9_3.png)
   Handling keterangan bahwa senjata terkini sedang digunakan
   
9. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_10_3.png)
   Dalam menu battle pertempuran, kita perlu memastikan bahwsannya bilaman player mengetikkan attack dengan berbagai kombinasi,misal ATtacK,atTAcK dsb.
   tidak akan memengaruhi berjalannya pertempuran.
   
10. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_11_3.png)
    Setelah pertempuran selesai maka player akan mendapatkan gold secara random, dalam kasus ini praktikan diberikan kebebasan untuk melakukan random reward & HP enemy logicnya.

11. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_12_3.png)
    Dalam kasus ini,jika player mencoba untuk melakukan pertempuran kembali maka HP musuh akan te-reset kembali,
    dan pertempuran atau battle akan diulangi ke tahap awal tanpa memengaruhi rewardnya yang sudah didapatkan(reward tidak akan 
    ikut te-reset)

12. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_13_3.png)
    Konfirmasi keterangan jikalau player keluar dari permainan(keluar dari program player.c)

13. ![1t](https://github.com/rzkcp/Sisop-3-2025-IT16/blob/f2917028734616e78cb559908578e617d08f8169/assets/ss_14_3.png)
    Kode program dari file shop.c berisikan list menu senjata yang tersedia, yang nantinya akan di-import ke dalam file dungeon.c
