Serwer Django:
Aby uruchomić serwer należy zmodyfikować plik `settings.py.sample` o uzupełnienie danych bazy zgodnie ze swoimi ustawieniami
i usunięcie rozszerzenie `.sample`.

#How To start:
By zaczac korzystac z virtualENV uzyj skryptu
'Serwer/py3/Scripts/activate'

Następnie w pliku setings.py appki Serwer zmien ustawienia swojej bazy danych (właśnie pracuję żeby w gruncie wyskoczyl tylko monit o haslo i nazwe towjej instancji albo zeby samo zalozylo baze jak masz zainstalowany serwer postresSQL)

Teraz wystarczy ze bedziesz uzywac 
'python manage.py <twoja_opcja>'

Istotne jest bys uzyl python przed manage poniewaz inaczej system uzyje domyslanego pythona dla rozserzenia .py a jesli nie bedzie nim Python3 to nie bedzie ciekawie xD 




##TODO:
- [] Add to grunt autconfig for being envrinoment independ like auto database CREATION for Postgress
- [] Starting writting documantation   
