# SOI LAB 4
## Monitory

W ramach tego laboratorium należało rozwiązać problem synchronizacji z użyciem monitorów. Jako iż są cztery dane współdzielone - składniki do tworzenia pierogów, zadanie wymagało stworzenia czterech monitorów - po jednym dla każdego składnika.

Implementacja bufora `PackageBufferMonitor` została wykonana z użyciem kodu z materiałów przedmiotu, oraz korzystając z biblioteki `queue`, tworząc za jej pomocą swoisty bufor który jest chroniony przed jednoczesnym użyciem przez wiele wątków za pomocą metod monitora `Hoare`.. Dodatkowo klasa ta ma dodatkowe metody zabezpieczające ją przed wstawianiem elementów do pełnego bufora, jak i prewentująca wyciąganie elementów z pustego bufora. Dodatkowo zostały zaimplementowane metody pozwalające na wstawianie elementów do bufora i wyciąganie z niego elementów.

Dzięki wykorzystaniu takiego rozwiązania złożoność problemu znacznie się zmniejszyła, a dodatkowo kod programu stał się znacznie bardziej czytelniejszy.