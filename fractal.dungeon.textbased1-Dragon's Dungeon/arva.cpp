#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <algorithm>
#include <random>
#include <conio.h>
#include <iomanip>
using namespace std;

// ======================= VARIABEL GLOBAL ======================= //
string nama, pilihan;
atomic<bool> sudahMenjawab(false);
atomic<bool> waktuHabis(false);
atomic<int> waktuTersisa(0);

// ======================= STRUCT SOAL ======================= //
struct Soal {
    string pertanyaan;
    string pilihanA;
    string pilihanB;
    string pilihanC;
    string pilihanD;
    string pilihanE;
    string jawabanBenar;
};

// ======================= DATABASE SOAL ======================= //
// PILIHAN GANDA BIASA (10 SOAL)
vector<Soal> soalPilihanGanda = {
    {"Perubahan cara hidup manusia praaksara dari nomaden menjadi menetap terjadi karena...", 
     "Meningkatnya kegiatan berburu", "Munculnya sistem kasta", "Ditemukannya cara bercocok tanam ", "Menurunnya suhu bumi", "Adanya peperangan antarkelompok", "c"},
    
    {"Penemuan kapak lonjong di Papua menunjukkan bahwa...", 
     "Seluruh wilayah Indonesia memiliki budaya sama", "Ada perbedaan budaya antara bagian barat dan timur ", "Papua lebih maju dalam logam", "Tidak ada pertukaran budaya", "Semua budaya datang dari India", "b"},
    
    {"Salah satu ciri manusia modern (Homo sapiens) dibanding manusia purba sebelumnya adalah…", 
     "Tidak mengenal api", "Hidup di gua", "Berpikir rasional dan mampu berbahasa", "Tidak mengenal alat batu", "Memiliki tubuh lebih kecil", "c"},
    
    {"Hubungan antara kondisi geografis Indonesia dengan kehidupan manusia praaksara adalah…", 
     "Geografi tidak memengaruhi", "Semua manusia praaksara hidup sama", "Lingkungan menentukan jenis alat dan pola hidup ", "Alat batu hanya ada di daerah pegunungan", "Laut tidak berpengaruh", "c"},
    
    {"Pengaruh kebudayaan India masuk ke Indonesia melalui jalur…", 
     "Perang dan penaklukan", "Penjajahan langsung", "Perdagangan dan hubungan budaya", "Penjajahan Eropa", "Migrasi besar-besaran", "c"},
    
    {"Berdasarkan teori arus balik, proses masuknya Hindu-Buddha ke Indonesia disebabkan oleh…", 
     "Penaklukan oleh India", "Pedagang India menyebarkan agama", "Bangsa Indonesia aktif belajar ke India ", "Penjajahan langsung", "Kewajiban kerajaan", "c"},
    
    {"Raja Mulawarman dari Kutai dikenal karena…", 
     "Menolak budaya India", "Melaksanakan upacara kurban emas", "Menyebarkan Islam", "Menulis naskah Ramayana", "Memindahkan ibu kota", "b"},
    
    {"Candi Borobudur merupakan bukti kemajuan peradaban di bidang…", 
     "Pertanian", "Arsitektur dan keagamaan ", "Perdagangan", "Militer", "Politik", "b"},
    
    {"Perbedaan corak antara kerajaan Mataram Kuno dan Sriwijaya adalah…", 
     "Mataram bercorak Buddha, Sriwijaya Hindu", "Mataram bercorak Hindu, Sriwijaya bercorak Buddha ", "Keduanya bercorak Islam", "Tidak ada perbedaan", "Sama-sama Buddha", "b"},
    
    {"Runtuhnya Majapahit disebabkan oleh…", 
     "Serangan VOC", "Perang saudara dan melemahnya pemerintahan ", "Serangan Mongol", "Letusan gunung berapi", "Krisis perdagangan", "b"}
};

// PILIHAN GANDA KOMPLEKS (10 SOAL)
vector<Soal> soalPilihanGandaKompleks = {
    {"Ciri-ciri manusia purba Pithecanthropus erectus di Indonesia ditunjukkan oleh pernyataan…", 
     "Berbadan tegap dan rahang kuat", "Volume otak sekitar 900 cc", "Hidup menetap di gua", "Sudah mengenal api", "Hidup secara nomaden", "abe"},
    
    {"Peninggalan budaya pada masa bercocok tanam ditunjukkan oleh contoh berikut…", 
     "Kapak genggam", "Menhir", "Nekara", "Lesung batu", "Kapak persegi", "bde"},
    
    {"Bukti bahwa bangsa Indonesia telah mengenal kehidupan sosial yang teratur pada masa praaksara adalah…", 
     "Adanya pembagian kerja", "Adanya pemimpin kelompok", "Penggunaan logam untuk alat", "Hidup berkelompok", "Adanya tulisan", "abd"},
    
    {"Faktor-faktor yang menyebabkan berkembangnya kerajaan-kerajaan Hindu-Buddha di Indonesia antara lain…", 
     "Letak geografis yang strategis", "Sikap terbuka masyarakat terhadap budaya luar", "Dukungan dari bangsa India", "Kekayaan sumber daya alam", "Sistem pertanian maju", "abd"},
    
    {"Bukti pengaruh Hindu-Buddha di bidang pemerintahan pada masa kerajaan-kerajaan Indonesia kuno adalah…", 
     "Adanya sistem kasta", "Raja dianggap sebagai titisan dewa", "Upacara pernikahan adat", "Adanya kitab hukum", "Penggunaan bahasa Sanskerta", "bde"},
    
    {"Faktor yang menyebabkan runtuhnya kerajaan-kerajaan bercorak Hindu-Buddha di Indonesia adalah…", 
     "Perkembangan agama Islam", "Perang saudara", "Melemahnya perdagangan", "Adanya kolonialisme Eropa", "Letak geografis yang terisolasi", "abd"},
    
    {"Nilai-nilai positif dari kehidupan masyarakat praaksara yang masih relevan hingga kini adalah…", 
     "Gotong royong", "Kepercayaan terhadap roh leluhur", "Musyawarah dalam kelompok", "Keterikatan dengan alam", "Sistem kasta", "acd"},
    
    {"Dampak positif masuknya Islam ke Indonesia adalah…", 
     "Berkembangnya pendidikan pesantren", "Munculnya kerajaan bercorak Islam", "Meningkatnya perdagangan antarwilayah", "Terhapusnya seluruh budaya lokal", "Penggunaan huruf Arab Melayu", "abcde"},
    
    {"Peran penting pelaut Nusantara dalam jalur perdagangan internasional masa lampau adalah…", 
     "Menjadi perantara antara Timur dan Barat", "Menjadi pelaut yang tangguh", "Menutup diri dari pedagang asing", "Membawa rempah-rempah ke luar negeri", "Mengembangkan pelabuhan dagang", "abcde"},
    
    {"Berikut ini termasuk peninggalan kerajaan bercorak Buddha di Indonesia, yaitu…", 
     "Candi Borobudur", "Candi Mendut", "Candi Prambanan", "Arca Buddha", "Kitab Negarakertagama", "abd"}
};
// ======================= TERMINAL SETTINGS ======================= //
void enableRawMode() {
    // Implementasi spesifik platform untuk mengaktifkan mode raw
    // Contoh di Windows menggunakan _setmode, di Linux menggunakan termios
    // Di sini kita gunakan conio.h untuk kesederhanaan (tidak perlu implementasi)
}

void disableRawMode() {
    // Restore terminal mode if needed; left empty for conio.h usage
}


// ======================= FUNGSI TAMPILAN ======================= //
void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

void judulUtama() {
    clearScreen();
    cout << "=========================================" << endl;
    cout << "              KUIS SEJARAH     " << endl;
    cout << "     Kelompok Tongseng Bu Soleh X-E4     " << endl;
    cout << "=========================================" << endl;
    cout << endl;
}

void tampilkanCredit() {
    clearScreen();
    cout << "\n==========================================" << endl;
    cout << "                TERIMA KASIH                  " << endl;
    cout << "===========================================" << endl;
    cout << "\nANGGOTA KELOMPOK TONGSENG BU SOLEH:\n" << endl;
    cout << "   1. Arva Al Ghifari Prabowo - Kepala Projek, Programmer, Presentasi" << endl;
    cout << "   2. Urbanus Advent Atmaja - Programmer, Presentasi" << endl;
    cout << "   3. Reva Hayyu Krisantya - Presentasi" << endl;
    cout << "\nMata Pelajaran: Informatika" << endl;
    cout << "Kelas: X-E4" << endl;
    cout << "Guru Pembimbing: Pak Razqa" << endl;
    cout << "\n==========================================" << endl;
    cout << "  Terima kasih sudah bermain, " << nama << "!" << endl;
    cout << "  Semoga menambah wawasan sejarah!" << endl;
    cout << "==========================================\n" << endl;
}

// ======================= FUNGSI LOADING ======================= //
void loadingAwal(int detik) {
    for (int i = detik; i > 0; i--) {
        cout << "\rKuis akan dimulai dalam " << i << " detik..." << flush;
        usleep(1000000);
    }
    cout << "\rMemulai kuis...                    " << endl << endl;
    usleep(500000);
}

void loadingSoal() {
    cout << "\nMemuat soal berikutnya";
    for (int i = 0; i < 3; i++) {
        cout << "." << flush;
        usleep(500000);
    }
    cout << "\n" << endl;
    usleep(300000);
}

void loadingHasil(int detik) {
    for (int i = detik; i > 0; i--) {
        cout << "\rMenghitung hasil dalam " << i << " detik..." << flush;
        usleep(1000000);
    }
    cout << "\rHasil siap!                         " << endl << endl;
}

// ======================= FUNGSI TIMER ======================= //
void updateTimer(int detik) {
    waktuTersisa = detik;
    for (int i = detik; i > 0; i--) {
        if (sudahMenjawab) return;
        waktuTersisa = i;
        usleep(1000000);
    }
    if (!sudahMenjawab) {
        waktuHabis = true;
        waktuTersisa = 0;
    }
}

// ======================= HELPERS ======================= //
// normalisasi jawaban: lowercase, ambil hanya huruf a-e, hapus duplikat, urutkan
string normalizeAnswer(const string &s) {
    string tmp;
    for (char ch : s) {
        char c = tolower((unsigned char)ch);
        if (c >= 'a' && c <= 'e') tmp.push_back(c);
    }
    sort(tmp.begin(), tmp.end());
    tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
    return tmp;
}

// ======================= FUNGSI INPUT DENGAN TIMER ======================= //
string inputDenganTimer(int waktuDetik) {
    sudahMenjawab = false;
    waktuHabis = false;
    string jawabanUser = "";
    
    cout << "\nKetik jawaban (a/b/c/d/e) lalu tekan ENTER" << endl;
    cout << "Waktu yang diberikan: " << waktuDetik << " detik" << endl;
    cout << "\nJawaban kamu: " << flush;
    
    thread timerThread(updateTimer, waktuDetik);
    
    enableRawMode();
    
    while (!waktuHabis && !sudahMenjawab) {
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r') { // ENTER
                sudahMenjawab = true;
                break;
            } else if (c == 8) { // BACKSPACE
                if (!jawabanUser.empty()) {
                    jawabanUser.pop_back();
                    cout << "\b \b" << flush;
                }
            } else if ((c >= 'a' && c <= 'e') || (c >= 'A' && c <= 'E')) {
                c = tolower(c);
                jawabanUser += c;
                cout << c << flush;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    
    sudahMenjawab = true;
    disableRawMode();
    
    timerThread.join();
    
    cout << endl;
    
    if (waktuHabis) {
        cout << "\nWAKTU HABIS! Jawaban dianggap salah.\n" << endl;
        jawabanUser = "salah";
    } else if (jawabanUser.empty()) {
        cout << "\nTidak ada jawaban yang dimasukkan.\n" << endl;
        jawabanUser = "salah";
    } else {
        cout << "\nJawaban tersimpan: " << jawabanUser << "\n" << endl;
    }
    
    return jawabanUser;
}

// ======================= FUNGSI PEMBUKA ======================= //
void pembukaan() {
    judulUtama();
    cout << "Silakan masukkan nama kamu terlebih dahulu\n" << endl;
    cout << "Nama: ";
    getline(cin, nama);
    
    clearScreen();
    judulUtama();
    cout << "Halo, " << nama << "!\n" << endl;
    cout << "Sebelum memulai, perhatikan petunjuk pengerjaan berikut:" << endl;
    cout << "   1. Terdapat 2 jenis soal: Pilihan Ganda Biasa dan Kompleks" << endl;
    cout << "   2. Waktu menjawab setiap soal adalah 30 detik" << endl;
    cout << "   3. Jika waktu habis, jawaban otomatis dianggap SALAH" << endl;
    cout << "   4. Ketik huruf pilihan (a/b/c/d/e) untuk pilihan ganda biasa " << endl;
    cout << "   5. Ketik lebih dari 1 huruf pilihan (misal: bde) untuk pilihan ganda kompleks, ingat! tidak ada spasi antar jawaban" << endl;
    cout << "   6. Urutan soal akan diacak setiap pengulangan dan bisa jadi acak atau berbeda dengan orang lain" << endl;
    cout << "   7. Waktu akan terus berjalan di latar belakang meskipun di tampilan waktu tampak berhenti" << endl;
    cout << "   8. Just For Fun Guys Chill" << endl;
    
    cout << "Apakah kamu siap memulai? (ketik Y/T): ";
    getline(cin, pilihan);
    
    if (pilihan == "Y" || pilihan == "y") {
        loadingAwal(5);
    } else {
        cout << "\nBaiklah, silakan belajar dulu ya!\n" << endl;
        exit(0);
    }
}

// ======================= FUNGSI TAMPILKAN HASIL ======================= //
void tampilkanHasil(int skor, int total, string jenis) {
    loadingHasil(3);
    
    double persentase = (skor * 100.0) / total;
    
    cout << "===========================================" << endl;
    cout << "         HASIL " << jenis << endl;
    cout << "===========================================" << endl;
    cout << "\nNama: " << nama << endl;
    cout << "Benar: " << skor << " dari " << total << " soal" << endl;
    cout << "Persentase: " << fixed << setprecision(1) << persentase << "%" << endl;
    cout << "\n";
    
    if (persentase == 100) {
        cout << "Makan apa kok pinter banget" << endl;
    } else if (persentase >= 80) {
        cout << "Gokil!" << endl;
    } else if (persentase >= 60) {
        cout << "Lumayan lah" << endl;
    } else if (persentase >= 40) {
        cout << "Ulang lagi sih" << endl;
    } else {
        cout << "Wassalam" << endl;
    }
    cout << "\n===========================================\n" << endl;
}

// ======================= FUNGSI KUIS UTAMA ======================= //
int jalankanKuis(vector<Soal>& daftarSoal, string jenisKuis) {
    int skor = 0;
    
    clearScreen();
    cout << "===========================================" << endl;
    cout << jenisKuis;
    
    cout << "===========================================" << endl;
    cout << "\n Mulai mengerjakan...\n" << endl;
    usleep(1000000);
    
    // Acak urutan soal
    random_device rd;
    mt19937 g(rd());
    shuffle(daftarSoal.begin(), daftarSoal.end(), g);
    
    for (size_t i = 0; i < daftarSoal.size(); i++) {
        clearScreen();
        
        cout << "===========================================" << endl;
        cout << " SOAL " << (i + 1) << " dari " << daftarSoal.size() << endl;
        cout << "===========================================" << endl;
        cout << "\n" << daftarSoal[i].pertanyaan << "\n" << endl;
        cout << "   a. " << daftarSoal[i].pilihanA << endl;
        cout << "   b. " << daftarSoal[i].pilihanB << endl;
        cout << "   c. " << daftarSoal[i].pilihanC << endl;
        cout << "   d. " << daftarSoal[i].pilihanD << endl;
        cout << "   e. " << daftarSoal[i].pilihanE << endl;
        
        string jawaban = inputDenganTimer(30);
        
        if (!waktuHabis && jawaban != "salah") {
            // bandingkan dengan kunci yang sudah dinormalisasi
            string normUser = normalizeAnswer(jawaban);
            string normKunci = normalizeAnswer(daftarSoal[i].jawabanBenar);
            if (!normKunci.empty() && normUser == normKunci) {
                skor++;
            }
        }
        
        if (i < daftarSoal.size() - 1) {
            loadingSoal();
        }
    }
    
    return skor;
}

// ======================= FUNGSI MENU UTAMA ======================= //
void menuUtama() {
    bool mainLagi = true;
    
    while (mainLagi) {
        // BAGIAN 1: PILIHAN GANDA BIASA
        int skorPG = jalankanKuis(soalPilihanGanda, "PILIHAN GANDA BIASA\n");
        tampilkanHasil(skorPG, soalPilihanGanda.size(), "PILIHAN GANDA BIASA");
        
        // Menu setelah PG Biasa //
        cout << "PILIHAN SELANJUTNYA:" << endl;
        cout << "   1. Lanjut ke Pilihan Ganda Kompleks" << endl;
        cout << "   2. Ulangi Pilihan Ganda Biasa" << endl;
        cout << "   3. Keluar dari kuis" << endl;
        cout << "\nPilih (1/2/3): ";
        getline(cin, pilihan);
        
        if (pilihan == "1") {
            // BAGIAN 2: PILIHAN GANDA KOMPLEKS //
            int skorPGK = jalankanKuis(soalPilihanGandaKompleks, "PILIHAN GANDA KOMPLEKS\n");
            tampilkanHasil(skorPGK, soalPilihanGandaKompleks.size(), "PILIHAN GANDA KOMPLEKS");
            
            // Menu setelah PG Kompleks //
            cout << "PILIHAN SELANJUTNYA:" << endl;
            cout << "   1. Ulangi Pilihan Ganda Biasa" << endl;
            cout << "   2. Ulangi Pilihan Ganda Kompleks" << endl;
            cout << "   3. Keluar dari kuis" << endl;
            cout << "\nPilih (1/2/3): ";
            getline(cin, pilihan);
            
            if (pilihan == "1") {
                continue; 
            } else if (pilihan == "2") {
                
                continue;
            } else {
                mainLagi = false;
            }
        } else if (pilihan == "2") {
            continue; 
        } else {
            mainLagi = false;
        }
    }
    
    tampilkanCredit();
}

// ======================= MAIN FUNCTION ======================= //
int main() {
    pembukaan();
    menuUtama();
    system("pause");
    cout << "Tekan ENTER untuk keluar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get(); // ✅ nunggu input ENTER

    return 0;
}