package main

import (
    "fmt"
    "math"
    "sync"
)

func ehPrimo(n int) bool {
    if n <= 1 {
        return false
    }
    if n == 2 {
        return true
    }
    if n%2 == 0 {
        return false
    }
    for i := 3; i <= int(math.Sqrt(float64(n)))+1; i += 2 {
        if n%i == 0 {
            return false
        }
    }
    return true
}

func findPrimes(nums <-chan int, results chan<- int, wg *sync.WaitGroup) {
    defer wg.Done()
    count := 0
    for n := range nums {
        if ehPrimo(n) {
            count++
        }
    }
    results <- count
}

func main() {
    var N, M int
    fmt.Print("Digite o valor de N (limite superior): ")
    fmt.Scan(&N)
    fmt.Print("Digite o valor de M (quantidade de goroutines): ")
    fmt.Scan(&M)

    nums := make(chan int, N)
    results := make(chan int, M)
    var wg sync.WaitGroup

    for i := 0; i < M; i++ {
        wg.Add(1)
        go findPrimes(nums, results, &wg)
    }

    for i := 1; i <= N; i++ {
        nums <- i
    }
    close(nums)

    go func() {
        wg.Wait()
        close(results)
    }()

    totalPrimes := 0
    for count := range results {
        totalPrimes += count
    }

    fmt.Printf("Quantidade total de números primos entre 1 e %d: %d\n", N, totalPrimes)
}
