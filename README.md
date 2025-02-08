# Arstai:PDA [WIP]

**Arstai:PDA** — частина інтегрованої екосистеми для організації та проведення рольових ігор у всесвіті S.T.A.L.K.E.R. Екосистема включає ПДА, електронні аномалії, симуляцію радіаційних зон, артефакти, детектори та інші елементи, що створюють унікальну атмосферу Зони Відчуження.

## Можливості
- **Сканування аномалій та загроз**: автоматичне оновлення даних раз на 1 секунду.
- **Аналіз RFID-карток**: зчитування карток та застосування відповідних ефектів.
- **Моніторинг стану гравця**: показники здоров'я, броні, рівня радіації тощо.
- **Веб-сервер для налаштувань**: зручний інтерфейс для доступу до параметрів.
- **Аудіо-відтворення**: підтримка програвання звукових сигналів.
- **Журнал подій**: запис, зберігання та перегляд логу подій.
- **Прошивка**: оновлення через веб-сервер або через адаптер CP2102.
- **RTC-модуль для відстеження часу**: забезпечення коректної роботи та реалізація викидів.

## Схема підключення
![promo](/Schem.png)

## Використані компоненти
- **ESP-WROOM-32** (16MB flash або Dev Module)
- **TFT-дисплей** 2.8" з контролером ILI9341
- **RTC модуль** DS3231
- **Аудіомодуль** DFPlayer
- **RFID-зчитувач** RC522
- **Динамік** 8 Ом, 2 Вт
- **MicroSD** (до 1 ГБ)
- **LoRa модуль** RA-01 або RA-02 + антена (433 МГц)


> **Примітка**: для повного тестування екосистеми знадобляться програматор RFID-карток і радіо-точка (збираються окремо).

## Екосистема Arstai:
- [Arstai:PDA](https://github.com/wainmurk/ARSTAI-PDA)
- [Arstai:Anomaly](https://github.com/wainmurk/ARSTAI-Anomaly)
- [Arstai:Uploader](https://github.com/wainmurk/ARSTAI-Uploader)
- [Arstai:Artifact](https://github.com/wainmurk/ARSTAL-Artifact)
