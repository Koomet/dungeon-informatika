import time
import datetime


def show_title():  #this thing is so cool -Habibi
    print("=[]=" * 11)
    print("\n" * 3)
    print(r"""
        ░█▄█░█▀▀░█▀█░█░█        
        ░█░█░█▀▀░█░█░█░█        
        ░▀░▀░▀▀▀░▀░▀░▀▀▀        
     ░█░█░█▀█░█▀█░▀█▀░▀█▀░█▀█
     ░█▀▄░█▀█░█░█░░█░░░█░░█░█
     ░▀░▀░▀░▀░▀░▀░░▀░░▀▀▀░▀░▀
    """)
    print("=[]=" * 11)
    print("=" * 30)
    print("SISTEM REKOMENDASI MENU KANTIN")
    print("=" * 30)

    print("\nLoading", end="")
    for _ in range(6):
        print(".", end="", flush=True)
        time.sleep(0.5)
    print("\n")


def waktu_otomatis(): #how much clock is it now? -Habibi
    jam = datetime.datetime.now().hour

    if 4 <= jam < 11:
        return "pagi"
    elif 11 <= jam < 15:
        return "siang"
    elif 15 <= jam < 18:
        return "sore"
    else:
        return "malam"


def rekomendasi_menu(waktu_makan):
    menu = {
        "pagi": ("Nasi Uduk", "Teh Hangat"),# i want this! -tachyon
        "siang": ("Nasi Goreng", "Es Teh"), #request from Tachyon
        "sore": ("Bakso", "Es Jeruk"),
        "malam": ("Mie Ayam", "Kopi Panas"), #requests from Manhattan cafe
    }

    return menu.get(waktu_makan, None)


def tampilkan_menu(waktu_makan):
    rekom = rekomendasi_menu(waktu_makan)

    if rekom:
        makanan, minuman = rekom
        print("\n" + "=" * 30)
        print(f"REKOMENDASI MENU WAKTU {waktu_makan.upper()}")
        print("=" * 30)
        print(f"Makanan : {makanan}")
        print(f"Minuman : {minuman}")
        print("=" * 30 + "\n")
    else:
        print("\n[Input tidak valid]\n")  


def narasi(teks, delay=0.03):
    for char in teks:
        print(char, end="", flush=True)
        time.sleep(delay)
    print()

def dialog(nama, teks, delay=0.03):
    print(f"{nama}: ", end="")
    narasi(f"\"{teks}\"", delay)

def lanjut(msg="\n[ENTER untuk lanjut] "):
    input(msg)
def scene_tachyon():  #scene khusus buat Tachyon ku tercinta tersayang 
    narasi("Siapa itu Agnes Tachyon?")
    lanjut()

    narasi("Bagi yang buta, dia adalah cahaya.")
    narasi("Bagi yang lapar, dia adalah roti.")
    narasi("Bagi yang sakit, dia adalah obat.")
    narasi("Bagi yang kesepian, dia adalah teman.")
    narasi("Bagi yang sedih, dia adalah kebahagiaan.")
   
    narasi("Bagi yang miskin, dia adalah harta.")
    narasi("Dan bagiku... dia adalah segala-galanya. ")
    narasi("Kalau Tachy punya sejuta fans, aku salah satunya.")
    narasi("Kalau Tachy cuma punya sepuluh fans, aku masih salah satunya.")
    narasi("Kalau Tachy cuma punya satu fan, ya itu aku.")
    narasi("Kalau Tachy gak punya fan sama sekali...")
    narasi("...berarti aku udah gak ada di dunia ini ")
    narasi("Kalau dunia melawan Tachy, maka aku akan melawan dunia.")
    
    narasi("Dia bukan cuma baik — dia LUAR BIASA!")
    narasi("Hebat, keren, indah, fantastis, memesona, menakjubkan, mengagumkan, super, mulia, dan penuh pesona!")
    narasi("Dia itu malaikat dari surga untuk mencerahkan dunia yang membosankan ini" )
    

    narasi("AKU CINTA KAMU, AGNES TACHYON!!! ♥♥♥")
    

    narasi("Dia adalah kesempurnaan yang menjelma jadi Uma Musume.")
    narasi("Langkahnya, iramanya, auranya — keindahan murni.")
    narasi("Bajunya elegan dan dia sangat jenius!")
    
    narasi("Kekuatan, kecerdasan, kemegahan, kecepatan, keanggunan, keceriaan, profesionalitas...")
    narasi("Semua sifat baik di dunia cocok buat dia ")

    narasi("Bahkan bintang yang berputar sejuta kali pun tak bisa menciptakan keajaiban seindah dia.")
    narasi("Seluruh semesta pun tak mampu menandingi cahaya ratu agung kita — Agnes Tachyon")
    lanjut()

    narasi("Untukmu, Tachyon... aku akan selalu ada.")
    lanjut("\nsekian Terima nasi ") 
 # pilihan manual

def menu_manual():
    while True:
        print("=" * 30)
        print("PILIH MENU MANUAL")
        print("=" * 30)

        print("1. Lihat menu pagi")
        print("2. Lihat menu siang")
        print("3. Lihat menu sore")
        print("4. Lihat menu malam")
        print("5. kembali ke menu utama")
        print("6. Keluar program")

        pilihan = input("\nMasukkan pilihan (1-5): ").strip()

        if pilihan == "1":
            tampilkan_menu("pagi")
        elif pilihan == "2":
            tampilkan_menu("siang")
        elif pilihan == "3":
            tampilkan_menu("sore")
        elif pilihan == "4":
            tampilkan_menu("malam")
        elif pilihan == "5":
            return    
        elif pilihan == "6":
            print("\nTerima kasih telah menggunakan sistem kantin otomatis!️")
            print("♥Tachyon pasti bangga sama kamu, Horazonka♥.\n")
            return "keluar"
        elif pilihan == "agnes tachyon ku tersayang":
           scene_tachyon()
           return "keluar"
           
        else:
            print("\nwoi cuma bisa 1-5!\n")
#credit for the developer
#developer: Habibi Muhammad Yunanto
#maybe just me? 
def show_credits():
    
    print("+------------------------------------+")
    print("| _  __    _                     _   |")
    print("|| |/ /___| |___ _ __  _ __  ___| |__|")
    print("|| ' </ -_) / _ \\ '  \\| '_ \\/ _ \\ / /|")
    print("||_|\\_\\___|_\\___/_|_|_| .__/\\___/_\\_\\|")
    print("|                     |_|            |")    
    print("|               ____                 |")
    print("|              | __ )                |")
    print("|                / /_                |")
    print("|              /_____|               |")     
    print("| -Habibi Muhammad Yunanto           |")
    print("| -Luthfi Siwi Wardhana              |")
    print("| -Nurrul Afifa Ghina Humaira        |")
    print("| -Aisha Mutia Kinanti Hartanto      |")
    print("+------------------------------------+")
                
# program untuk ya gitu deh
def main():
    show_title()

    print("Tanggal & Waktu Saat Ini:")
    now = datetime.datetime.now()
    print(now.strftime("%A, %d %B %Y — %H:%M:%S\n"))

    while True:
        print("=" * 30)
        print("MENU UTAMA")
        print("=" * 30)

        print("1. MENU SEKARANG APA?(berdasarkan jam)")
        print("2. AKU MAU LIAT MENU WAKTU LAIN")
        print("3. KELUAR PROGRAM + CREDITS")

        pilihan = input("\n Jadi lu pilh antara (1-3): ").strip()

        if pilihan == "1":
            waktu = waktu_otomatis()              
            tampilkan_menu(waktu)
        elif pilihan == "2":
            hasil = menu_manual()
            if hasil == "keluar":
                lanjut()
                break
        elif pilihan == "3":
            print("\nTerima kasih telah menggunakan sistem kantin otomatis!️")
            show_credits()
            print("♥Tachyon pasti bangga sama kamu, Horazonka♥.\n")
            lanjut()
            break
        elif pilihan == "aku sayang Tachyon":
          narasi("\nAku juga sayang kamu Horazonka!")
          print("                -Agnes Tachyon♥\n")
            
        
        else:
            print("\ninput mu salah! Pilihan cuma 1-3!\n")
            
#little bit of this little bit of that
#and now its complete
#wait what if there is....., that is last massage before the massive bug

if __name__ == "__main__":
    main()
    #the end , now i can rest at least for today -Habibi
