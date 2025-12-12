#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

void pressEnter() {
    cout << "\n( Tekan ENTER untuk melanjutkan... )" << endl;
    // clear the input buffer until newline
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void membersilver() {
    cout << "\n================ MEMBER ZBW CLUB ================" << endl;
    cout << "MEMBER GOLD:" << endl;
    cout << "1.asep" << endl;
    cout << "2.budi" << endl;
    cout << "3.caca" << endl;       

}

void membergold() {
    cout << "\n================ MEMBER ZBW CLUB ================" << endl;
    cout << "MEMBER SILVER:" << endl;
    cout << "1.deni" << endl;
    cout << "2.eka" << endl;
    cout << "3.fani" << endl;
    cout << "4.gilang" << endl;
    cout << "5.hani" << endl;       
    
}

int main() {
    // ====== LOGIN KASIR ======
    string username, password;
    const string userKasir = "zbw";
    const string passKasir = "zbw";
    bool login = false;
    int percobaan = 3;

    cout << "==================================================" << endl;
    cout << "              SISTEM KASIR ZBW CLUB               " << endl;
    cout << "         EXPERIENCE | STYLE | ENERGY              " << endl;
    cout << "==================================================" << endl;

    while (percobaan > 0) {
        cout << "\nMasukkan Username: ";
        cin >> username;
        cout << "Masukkan Password: ";
        cin >> password;

        if (username == userKasir && password == passKasir) {
            login = true;
            cout << "\nLogin berhasil! Selamat datang, " << username << "!" << endl;
            break;
        } else {
            percobaan--;
            cout << "Username atau password salah! Sisa percobaan: " << percobaan << endl;
        }
    }

    if (!login) {
        cout << "\nAkses ditolak! Program keluar otomatis." << endl;
        return 0;
    }

    // ====== VARIABEL ======
    int pilihan, jumlah, tipeMember, metode;
    long harga, total, bayar = 0, kembalian = 0;
    double diskon = 0.0, diskonMember = 0.0, totalSetelahDiskon;
    int poin = 0;
    char ulang;
    string namaMenu, namaMember, metodeBayar;
    ofstream notaFile;

    double totalOmzet = 0;
    int totalTransaksi = 0;

    cout << fixed << setprecision(0);

    // ====== LOOP TRANSAKSI ======
    do {
        cout << "\n==================================================" << endl;
        cout << "              MENU TRANSAKSI ZBW CLUB             " << endl;
        cout << "==================================================" << endl;
        cout << "Apakah pelanggan memiliki member?" << endl;
        cout << "0. Non-Member" << endl;
        cout << "1. Silver Member (+5% diskon)" << endl;
        cout << "2. Gold Member (+10% diskon)" << endl;
        cout << "Lihat member yang terdaftar (ketik 3): " << endl;
        cout << "Masukkan tipe member (0-2): ";
        cin >> tipeMember;

        if (tipeMember == 1) namaMember = "Silver Member";
        else if (tipeMember == 2) namaMember = "Gold Member";
        else if (tipeMember == 3) {
            membersilver();
            membergold();
            cout << "\nTekan ENTER untuk kembali ke menu transaksi...";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ulang;
        }
        else namaMember = "Non-Member";

        cout << "\n=================== MENU ZBW CLUB ===================" << endl;
        cout << ">> PAKET SPESIAL <<" << endl;
        cout << " 1. The ZBW Experience   - Rp 2.000.000" << endl;
        cout << " 2. Triple ZBW Shot      - Rp 1.500.000" << endl;
        cout << " 3. Trinity Night        - Rp   755.000" << endl;
        cout << " 4. Zabiws Dream         - Rp   630.000" << endl;
        cout << "\n>> MINUMAN SIGNATURE <<" << endl;
        cout << " 5. Wizab Breeze         - Rp   300.000" << endl;
        cout << " 6. Bizian Fizz          - Rp   300.000" << endl;
        cout << " 7. Zawi Cooler          - Rp   300.000" << endl;
        cout << " 8. Zenergy Spark        - Rp   275.000" << endl;
        cout << " 9. Mystic Mojito        - Rp   250.000" << endl;
        cout << "\n>> MAKANAN FAVORIT <<" << endl;
        cout << "10. Zaks Fries           - Rp    45.000" << endl;
        cout << "11. Bisy Wings           - Rp    50.000" << endl;
        cout << "12. Wids Sliders         - Rp    58.000" << endl;
        cout << "13. Cheezy Volcano       - Rp    60.000" << endl;
        cout << "14. Spicy Z-Burger       - Rp    80.000" << endl;
        cout << "=====================================================" << endl;

        cout << "Masukkan pilihan menu Anda (1-14): ";
        cin >> pilihan;

        switch (pilihan) {
            case 1: namaMenu = "The ZBW Experience"; harga = 2000000; break;
            case 2: namaMenu = "Triple ZBW Shot"; harga = 1500000; break;
            case 3: namaMenu = "Trinity Night"; harga = 755000; break;
            case 4: namaMenu = "Zabiws Dream"; harga = 630000; break;
            case 5: namaMenu = "Wizab Breeze"; harga = 300000; break;
            case 6: namaMenu = "Bizian Fizz"; harga = 300000; break;
            case 7: namaMenu = "Zawi Cooler"; harga = 300000; break;
            case 8: namaMenu = "Zenergy Spark"; harga = 275000; break;
            case 9: namaMenu = "Mystic Mojito"; harga = 250000; break;
            case 10: namaMenu = "Zaks Fries"; harga = 45000; break;
            case 11: namaMenu = "Bisy Wings"; harga = 50000; break;
            case 12: namaMenu = "Wids Sliders"; harga = 58000; break;
            case 13: namaMenu = "Cheezy Volcano"; harga = 60000; break;
            case 14: namaMenu = "Spicy Z-Burger"; harga = 80000; break;
            default:
                cout << "Pilihan tidak tersedia!" << endl;
                harga = 0;
        }

        if (harga > 0) {
            cout << "Masukkan jumlah yang ingin dibeli: ";
            cin >> jumlah;

            total = harga * jumlah;

            // ===== DISKON =====
            if (total > 1000000) {
                diskon = 0.20 * total;
                cout << "\nDiskon dasar 20% untuk pembelian di atas Rp1.000.000!" << endl;
            } else if (total > 500000) {
                diskon = 0.10 * total;
                cout << "\n Diskon dasar 10% untuk pembelian di atas Rp500.000!" << endl;
            } else diskon = 0.0;

            if (tipeMember == 1) {
                diskonMember = 0.05 * total;
                cout << " Silver Member: Diskon tambahan 5%!" << endl;
            } else if (tipeMember == 2) {
                diskonMember = 0.10 * total;
                cout << " Gold Member: Diskon tambahan 10%!" << endl;
            } else diskonMember = 0.0;

            totalSetelahDiskon = total - (diskon + diskonMember);

            // ===== CETAK STRUK SEMENTARA (BELUM BAYAR) =====
            cout << "\n============== PREVIEW PEMBAYARAN ===============" << endl;
            cout << left << setw(25) << "Menu" << ": " << namaMenu << endl;
            cout << left << setw(25) << "Jumlah" << ": " << jumlah << endl;
            cout << left << setw(25) << "Total Belanja" << ": Rp " << total << endl;
            cout << left << setw(25) << "Diskon Total" << ": Rp " << (diskon + diskonMember) << endl;
            cout << left << setw(25) << "Total Bayar" << ": Rp " << totalSetelahDiskon << endl;
            cout << "==================================================" << endl;

            // ===== PEMBAYARAN =====
            cout << "\nPilih metode pembayaran:" << endl;
            cout << "1. Cash" << endl;
            cout << "2. QRIS (Scan QR)" << endl;
            cout << "Masukkan pilihan (1/2): ";
            cin >> metode;

            if (metode == 2) {
                metodeBayar = "QRIS";
                cout << "\n Silahkan scan kode QR berikut:" << endl;
                cout << "====================================" << endl;
                cout << "|     [ QR ZBW CLUB ]     |" << endl;
                cout << "|  ID: ZBW123456PAYMENT            |" << endl;
                cout << "|  NOMINAL: Rp " << totalSetelahDiskon << "           |" << endl;
                cout << "====================================" << endl;
                cout << "Pembayaran QRIS berhasil!" << endl;
                bayar = totalSetelahDiskon;
                kembalian = 0;
            } else {
                metodeBayar = "Cash";
                cout << "Masukkan uang pembayaran: Rp ";
                cin >> bayar;
                if (bayar < totalSetelahDiskon) {
                    cout << "Uang tidak cukup! Transaksi batal." << endl;
                    continue;
                }
                kembalian = bayar - totalSetelahDiskon;
            }
            string pelanggan

            // ===== CETAK STRUK AKHIR =====
            string labelBayar = "Dibayar (" + metodeBayar + ")";
            cout << "\n=============== RINCIAN PEMBAYARAN ===============" << endl;
            cout << left << setw(25) << pelanggan << endl;
            cout << left << setw(25) << "Total Belanja" << ": Rp " << total << endl;
            cout << left << setw(25) << "Diskon Total" << ": Rp " << (diskon + diskonMember) << endl;
            cout << left << setw(25) << "Total Bayar" << ": Rp " << totalSetelahDiskon << endl;
            cout << left << setw(25) << labelBayar << ": Rp " << bayar << endl;
            cout << left << setw(25) << "Kembalian" << ": Rp " << kembalian << endl;
            cout << "==================================================" << endl;

            totalOmzet += totalSetelahDiskon;
            totalTransaksi++;
        }

        cout << "\nApakah ingin melanjutkan transaksi? (y/n): ";
        cin >> ulang;

    } while (ulang == 'y' || ulang == 'Y');

    // ===== LAPORAN HARIAN =====
    cout << "\n==================================================" << endl;
    cout << "                LAPORAN HARIAN KASIR              " << endl;
    cout << "==================================================" << endl;
    cout << left << setw(25) << "Total Transaksi" << ": " << totalTransaksi << endl;
    cout << left << setw(25) << "Total Omzet Hari Ini" << ": Rp " << totalOmzet << endl;
    cout << "==================================================" << endl;
    cout << "Kasir " << username << " telah logout dari sistem." << endl;
    cout << "Terima kasih sudah menggunakan sistem kasir ZBW CLUB!" << endl;
    cout << "==================================================" << endl;

    return 0;
}
