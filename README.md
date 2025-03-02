# ArduinoProjects

# Ceas cu senzori - schetch_oct23a.ino

## Descriere proiect

Acesta este un proiect care afișează pe un afișaj LED, informații referitoare la timp, temperatură, umiditate și starea senzorilor de gaz și flacără. Sistemul include alarme sonore pentru avertizarea apariției situațiilor anormale de concentrație a unor gaze din aer sau la detectarea focului.

## Funcționalități

- Afișarea timpului și datei
- Citirea și afișarea temperaturii și umidității
- Detectarea gazului și flăcărilor cu activarea alarmei sonore
- Moduri diferite de afișare în funcție de informații

## Componente hardware necesare
*Fiecare componentă are înserat un link către produs, unde se pot regăsi mai multe informații despre produs, cum ar fi caracteristici tehnice, instrucțiuni de utilizare etc.

*Pentru a accesa site-ul pricipal de pe care s-au achiziționat componentele electronice apăsați [aici](https://www.optimusdigital.ro).

- Placă de dezvoltare: [Pulsivo Wireless, compatibilă cu ESP32 și BLE](https://www.optimusdigital.ro/ro/placi-cu-esp32/12933-placa-de-dezvoltare-plusivo-wireless-compatibila-cu-esp32-si-ble.html?search_query=esp32&results=28) 
![image](https://github.com/user-attachments/assets/f99be17a-e59f-439f-b10c-78fb3cdf6495)
- Senzor de temperatură și umiditate: [DHT22](https://www.optimusdigital.ro/ro/senzori-senzori-de-temperatura/3157-senzor-de-temperatura-i-umiditate-dht22am2302b.html?search_query=dht22&results=6)
- Modul RTC: [DS3231](https://www.optimusdigital.ro/ro/altele/1102-modul-cu-ceas-in-timp-real-ds3231.html?search_query=ds3231&results=6)
- Senzor de gaz (gaz metan): [MQ-4](https://www.optimusdigital.ro/ro/senzori-de-gaze/1130-modul-senzor-de-gaz-mq-4.html?search_query=senzor+gaz&results=34)
- Senzor de gaz (monoxid de carbon): [MQ-7](https://www.optimusdigital.ro/ro/senzori-de-gaze/1127-modul-senzor-de-gaz-mq-7.html?search_query=senzor+gaz&results=34)
- [Senzor de flacără infraroșu](https://www.optimusdigital.ro/ro/senzori-senzori-optici/110-modul-senzor-de-flacara-.html?search_query=flacara&results=3)
- Modul cu matrici LED: [MAX7219](https://www.optimusdigital.ro/ro/optoelectronice-matrice-de-led-uri/4154-modul-max7219-cu-4-matrice-led.html) (display-ul este format din 16 astfel de module, așezate pe două rânduri)
- [Buzzer activ](https://www.optimusdigital.ro/ro/audio-buzzere/12513-pcb-mounted-active-buzzer-module.html?search_query=buzzer&results=61) pentru alarme sonore
- Fire de conexiune

## Biblioteci necesare

### MD_MAX72xx
-> bibliotecă ce oferă controlul de bază al modulelor MAX7219 (configurarea matricei LEI, aprinderea și stingerea LED-urilor, controlul intesității luminoase, gestionarea comunicației SPI)

### MD_Parola.h
-> bibliotecă ce adaugă funcții avansate pentru afișaj (precum afișare animată, efecte vizuale sau gestionarea automată a actualizării afișajului)

### SPI.h
-> bibliotecă necesară pentru comunicarea *SPI* cu afișaju LED

### Wire.h
-> bibliotecă pentru comunicarea *I2C*, necesară pentru interacțiunea cu modulul RTC

### MD_DS3231.h
-> bibliotecă pentru utilizarea ceasului de timp real (RTC)

### DHT.h
-> este bibliotecă utilizată pentru citirea datelor transmise de senzorul DHT22

### Font_Data.h
-> este un fișier personalizat ce conține fonturi utilizate de afișajul LED

## Conectare componente hardware, definire și configurare pini 

### Afișajul LED 
-> Se alege tipul specific pentru modulele LED (HARDWARE_TYPE): *MD_MAX72XX::FC16_HW*. FC16_HW ajută biblioteca MD_MAX72XX să trimită corect datele către matricea LED, ținând cont de orientarea matricii și de configurația pinilor pe modul.

-> Pentru formarea unui afișaj de dimensiuni mai mari am conectat mai multe module MAX7219 în serie. Numărul de module este indicat prin macro-ul *MAX_DEVICES* (în acest caz este vorba de 16 module conectate în serie).

-> Se alimentează la 5V, conectând la pinul corespunzător tensiunii de 5V al plăcii de dezvoltare. Pentru completarea circuitului, pinul GND al afișajului se conectează la pinul GND al plăcii, stabilind referința comună pentru masă.

-> Pinii utilizați pentru comunicarea SPI cu afișajul LED sunt:
  1. CLK: Conexiunea pentru semnalul de ceas se face la pinul GIOP18 al plăcii, fiind definit în cod prin macro-ul *CLK_PIN*.
  2. DIN (Data In): Acest pin este responsabil pentru transmiterea datelor către afișaj, este conectat la pinul GIOP23 al plăcii și este definit în program ca *DATA_PIN*.
  3. CS (Chip Select): Pinul definit în cod ca *CS_PIN* se ocupă de selectarea afișajului în timpul comunicării SPI și se conectează la pinul GIOP5 al plăcii.

### Modulul RTC (Real Time Clock) - DS3231

-> Se conectează pinul VCC al modulului la pinul de 5V al plăcii și pinul GND al modulului la GND-ul plăcii.

-> Pinii utilizați pentru comunicare:
  1. SDA (Serial Data): Modulul RTC comunică cu placa de dezvoltate (transmite și recepționează date pe magistrala I2C), conectând *SDA* la *GIOP21* de pe placă. Acest pin este definit în program ca *RTC_SDA_PIN*.
  2. SCL (Serial Clock): Pinul utilizat pentru semnalul de ceas în comunicarea I2C se conectează la pinul *GIOP22* al plăcii. Acesta sincronizează transferul de date ditnre RTC și microcontroller. În program se găsește definit ca *RTC_SCL_PIN*.

-> Pentru ca modulul RTC să asigure menținerea informațiilor referitoare la timp chiar și atunci când modulul este deconectat de la alimentarea principală, am adăugat o baterie (am utilizat o baterie *CR 2032*).

### Senzori și alarme

- Senzor de temperatură și umiditate - DHT22

  -> Se alimentează în mod asemeănător cu celelalte componente de mai sus.

  -> Pinul de date al senzorului DHT22 se conectează la pinul *GIOP4* al plăcii, permițând astfel transmiterea informațiilor măsurate de senzor spre microcontroler. Acesta este definit ca *TEMP_HUM_PIN*. Pentru a oferi o claritate codului voi defini și tipul senzorului utilizat prin macro-ul *DHTTYPE* (în cazul în care doresc să utilizez alt tip de senzor, va fi nevoie de o singură modificare).
- Senzor de flacără

  -> Pinul VCC al senzorului se conectează la pinul *VIN 5V* al plăcii, iar pinul GND la GND-ul plăcii. Pinul de date *DO* (Digital Output) se conectează la plăcuță prin intermediul *GIOP15*. Acesta este definit ca *FIRE_PIN*. În momentul în care se detectează flacără se va aprinde și un led pe modulul senzorului.
- Senzori gaze

  -> Atât pentru senzorul *MQ-7*, cât și pentru senzorul *MQ-4*, alimentarea se face în mod asemănător, conectând pinul *VCC* al senzorului la *VIN 5V* și GND la masa plăcii.

  -> Pentru transmiterea datelor se vor utiliza pinii digitali ai senzorilor: DO. Pentru *MQ-7* am conectat *DO* la *GIOP36*, iar pentru *MQ-4* la *GIOP39*.

  -> Acești doi senzori sunt definiți în cod ca *MQ7_PIN*, respectiv *MQ4_PIN*.
- Buzzer

  -> Acesta este utilizat, în acest program, pentru a alerta utilizatorul de apariția unei erori.
  -> Se conectează pinul *-* (*minus*) al alarmei la pinul *GND* al plăcii.

  -> Pinul corespunzător semnalului (*S*) se conectează la *GIOP0* și este definit în program ca *BUZZER_PIN*.
- Led-ul de pe placa de dezvoltare

  -> Acesta este utilizat pentru a emite semnale luminoase odată cu activarea alarmei sonore sau în caz de anumite erori. Pinul la care este conectat ledul este definit ca *LED_PIN* și utilizează *GIOP2*.

 ### Inițializare obiecte
Pentru a putea configura corect dispozitivele externe este nevoie de crearea acestor obiecte în mediul de programare astfel încât să îndeplinească funcțiile pentru care au fost proiectate.
- Pentru a putea controla afișajul LED se creează obiectul *myDisplay* folosind biblioteca *MD_Parola* și clasa cu același nume. Se specifică tipul de hardware al afișajului, pinul de selecție (CS_PIN) și numărul maxim de dispozitive conectate în lanț. Inițializarea acestui obiect permite utilizarea tuturor metodelor clasei *MD_Parola*.
- Ca să fie posibilă utilizarea funcțiilor ce interacționează cu modulul RTC s-a creat un obiect de tipul MD_DS3231, numit *rtc*.
- Pentru a interacționa cu senzorul de temperatură și umiditate se creează un obiect de tipul *DHT* și se inițializează cu numele pinul corespunzător (TEMP_HUM_PIN) și tipul senzorului (DHTTYPE).
