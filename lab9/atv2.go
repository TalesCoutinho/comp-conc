package main

import (
    "fmt"
)

func goroutine(str chan string) {
    fmt.Println(<-str) // Recebe a primeira mensagem
    str <- "Oi Main, bom dia, tudo bem?" // Envia resposta para Main
    fmt.Println(<-str) // Recebe a segunda mensagem
    str <- "Certo, entendido." // Envia resposta final para Main
    fmt.Println("finalizando") // Imprime finalização
}

func main() {
    str := make(chan string)
    go goroutine(str)

    str <- "Olá, Goroutine, bom dia!" // Main envia para Goroutine
    fmt.Println(<-str) // Recebe resposta da Goroutine
    str <- "Tudo bem! Vou terminar tá?" // Main envia mensagem final
    fmt.Println(<-str) // Recebe resposta final da Goroutine
    fmt.Println("finalizando") // Imprime finalização
}

