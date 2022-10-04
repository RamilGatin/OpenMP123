#include <iostream>
#include <omp.h>
#include <string>
#include <windows.h>


using namespace std;

void fillWithRandomInts(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand();
    }
}

void firstTask() {
#pragma omp parallel num_threads(8)
    {
        printf("Hello world. Thread: %d\n", omp_get_thread_num());
    }
}


void secondTask() {
    omp_set_num_threads(3);
#pragma omp parallel for if(omp_get_max_threads() > 2)
    for (int i = 0; i < 1000000; i++)
    {
        if (i % 200000 == 0)
        {
            printf("Hello world. Thread: %d\n", omp_get_thread_num());
        }
    }
    printf("\n\n\n");

    omp_set_num_threads(2);
#pragma omp parallel for if(omp_get_max_threads() > 2)
    for (int i = 0; i < 1000000; i++)
    {
        if (i % 200000 == 0)
        {
            printf("Hello world. Thread: %d\n", omp_get_thread_num());
        }
    }
}

void thirdTask()
{
    int a = 2, b = 15;

    printf("Before first block a = %d, b = %d\n", a, b);
#pragma omp parallel num_threads(2) private(a) firstprivate(b)
    {
        a = 3, b = 15;
        a = a + omp_get_thread_num();
        b = b + omp_get_thread_num();
        printf("\tIn first block after increment a = %d, b = %d\n\n", a, b);
    }
    printf("After first block a = %d, b = %d\n", a, b);

    printf("Before second block a = %d, b = %d\n", a, b);
#pragma omp parallel num_threads(4) shared(a) private(b)
    {
        b = 15;
        a -= omp_get_thread_num();
        b -= omp_get_thread_num();
        printf("\tIn second block after decrement a = %d, b = %d, threads_num = %d\n\n", a, b, omp_get_thread_num());
    }
    printf("After second block a = %d, b = %d\n", a, b);

}

void fourthTask()
{
    const int arrLength = 100000;
    int a[arrLength], b[arrLength];
    fillWithRandomInts(a, arrLength);
    fillWithRandomInts(b, arrLength);

    int min = INT_MAX;
    int max = INT_MIN;
#pragma omp parallel num_threads(2) shared(min, max)
    {
#pragma omp master
        {
            printf("%d\n", omp_get_thread_num());
            for (int i = 0; i < arrLength; i++)
            {
                if (a[i] < min)
                {
                    min = a[i];
                }
            }
        }
#pragma omp single nowait
        {
            printf("%d\n", omp_get_thread_num());
            for (int i = 0; i < arrLength; i++)
            {
                if (b[i] > max) max = b[i];
            }
        }
    }
    printf("max = %d, min = %d", max, min);
}

void fifthTask() {
    int d[6][8];
    for (int i = 0; i < 6; i++) {
        fillWithRandomInts(d[i], 8);
    }

#pragma omp parallel
    {
#pragma omp sections
        {
#pragma omp section
            {
                double avg = 0;
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        avg += d[i][j] / 48.0;
                    }
                }
                printf("Thread: %d, avg = %f\n", omp_get_thread_num(), avg);
            }
#pragma omp section
            {
                int min = INT_MAX;
                int max = INT_MIN;
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if (d[i][j] > max) max = d[i][j];
                        if (d[i][j] < min) min = d[i][j];

                    }
                }
                printf("Thread: %d, max = %d, min = %d\n", omp_get_thread_num(), max, min);
            }
#pragma omp section
            {
                int amount = 0;
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if (d[i][j] % 3 == 0) amount++;
                    }
                }
                printf("Thread: %d, amount = %d\n", omp_get_thread_num(), amount);
            }
        }
    }
}
void sixthTask() {
    int a[100];
    double sumWithReduction = 0;
    double sumWithoutReduction = 0;
    fillWithRandomInts(a, 100);
#pragma omp parallel for reduction(+ : sumWithReduction)
    for (int i = 0; i < 100; i++)
    {
        sumWithReduction += a[i];
    }

#pragma omp parallel for
    for (int i = 0; i < 100; i++)
    {
        sumWithoutReduction += a[i];
    }

    double sumSeq = 0;
    for (int i = 0; i < 100; i++)
    {
        sumSeq += a[i];
    }

    printf("avg when sum calculated with reduction = %f\n", sumWithReduction / 100);
    printf("avg when sum calculated without reduction = %f\n", sumWithoutReduction / 100);
    printf("avg when sum calculated sequentially = %f\n", sumSeq / 100);
}

void seventhTask() {
    int a[12], b[12], c[12];

#pragma omp parallel for num_threads(3) schedule(static, 4)
    for (int i = 0; i < 12; i++) {
        a[i] = rand();
        b[i] = rand();
        printf("a[i] = %d, b[i] = %d, thread = %d, num_threads = %d\n", a[i], b[i], omp_get_thread_num(), omp_get_num_threads());
    }

#pragma omp parallel for num_threads(4) schedule(dynamic,4)
    for (int i = 0; i < 12; i++) {
        c[i] = a[i] + b[i];
        printf("c[i] = %d, thread = %d, num_threads = %d\n", c[i], omp_get_thread_num(), omp_get_num_threads());
    }

}
void eightTask() {
    int a[16000];
    for (int i = 0; i < 16000; i++)
    {
        a[i] = i;
    }
    double res[16000];

    // DYNAMIC WITH BATCH SIZE 4
    double before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(dynamic,4)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    double after = omp_get_wtime();
    printf("Dynamic, with batch size = %d. Execution time: %f\n", 4, after - before);

    // DYNAMIC WITH BATCH SIZE 16
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(dynamic,16)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("Dynamic, with batch size = %d. Execution time: %f\n", 16, after - before);

    // STATIC WITH BATCH SIZE 4
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(static,4)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("STATIC, with batch size = %d. Execution time: %f\n", 4, after - before);

    // STATIC WITH BATCH SIZE 16
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(static,16)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("STATIC, with batch size = %d. Execution time: %f\n", 16, after - before);

    // GUIDED WITH BATCH SIZE 4
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(guided,4)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("GUIDED, with batch size = %d. Execution time: %f\n", 4, after - before);

    // GUIDED WITH BATCH SIZE 16
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(guided,16)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("GUIDED, with batch size = %d. Execution time: %f\n", 16, after - before);

    // RUNTIME WITH BATCH SIZE 4
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(runtime)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("RUNTIME, with batch size = %d. Execution time: %f\n", 4, after - before);

    // RUNTIME WITH BATCH SIZE 16
    before = omp_get_wtime();
#pragma omp parallel for num_threads(8) schedule(runtime)
    for (int i = 1; i < 15999; i++) {
        res[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
    }
    after = omp_get_wtime();
    printf("RUNTIME, with batch size = %d. Execution time: %f\n", 16, after - before);

}
void nine() {
    int const matrix_size = 10000;
    long * matrix = new long[matrix_size * matrix_size];
    long * vector = new long[matrix_size];
    long * results = new long[matrix_size];

    for (size_t i = 0; i < matrix_size; i++)
    {
        results[i] = 0;
        vector[i] = rand();
        for (int j = 0; j < matrix_size; j++)
        {
            matrix[i * matrix_size + j] = rand();
        }
    }

    double before = omp_get_wtime();
#pragma omp parallel
    {
        long sLocal = 0;
        int i, j;
        for (i = 0; i < matrix_size; i++) {
#pragma omp for schedule(dynamic, 50)
            for (j = 0; j < matrix_size; j++) {
                sLocal += matrix[i*matrix_size + j] * vector[j];
            }
#pragma omp critical
            {
                results[i] += sLocal;
                sLocal = 0;
            }
        }
    }

    double after = omp_get_wtime();
    printf("Parallel %f\n", after - before);

    before = omp_get_wtime();

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            results[i] = matrix[i*matrix_size + j] * vector[j];
        }
    }

    after = omp_get_wtime();
    printf("posled %f", after - before);


}

void ten() {
    int d[6][8];
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            d[i][j] = rand();
            printf("%d ", d[i][j]);
        }
        printf("\n");
    }

    int min = INT16_MAX;
    int max = INT16_MIN;

#pragma omp parallel for num_threads(6)
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (d[i][j] < min)
            {
#pragma omp critical
                if (d[i][j] < min)
                {
                    min = d[i][j];
                }
            }
            if (d[i][j] > max)
            {
#pragma omp critical
                if (d[i][j] > max)
                {
                    max = d[i][j];
                }
            }
        }
    }
    printf("Min: %d \nMax: %d", min, max);
}
void eleven() {
    int a[30];
    fillWithRandomInts(a, 30);
    int cnt = 0;
#pragma omp parallel for
    for (int i = 0; i < 30; i++) {
        if (a[i] % 9 == 0)
        {
#pragma omp atomic
            cnt++;
        }
    }
    for (int i = 0; i < 30; i++)
    {
        if (a[i] % 9 == 0) {
            printf("%d ", a[i]);
        }
    }
    printf("cnt: %d", cnt);

}

void twelve() {
    int d[200];
    for (int i = 0; i < 200; i++)
    {
        d[i] = rand();
    }

    int max = INT16_MIN;

#pragma omp parallel for num_threads(6)
    for (int i = 0; i < 200; i++)
    {
        if (d[i] > max && d[i] % 7 == 0)
        {
#pragma omp critical
            if (d[i] > max)
            {
                max = d[i];
            }
        }
    }
    if (max == INT16_MIN)
    {
        printf("There is no maximum (((");
    }
    else {
        printf("Max: %d", max);
    }
}
void thirteen_first() {

    int threads = 8;

#pragma omp parallel num_threads(threads)
    for (int i = threads - 1; i >= 0; i--) {
#pragma omp barrier
        if (i == omp_get_thread_num()) {
            printf("%d", i);
        }
    }
}

void thirteen_second(){


#pragma omp parallel
        {
            int nthreads = omp_get_num_threads();
            int i = 7;
            while (i >= 0)
            {
#pragma omp barrier
                {
                    if (i == omp_get_thread_num())
                    {
#pragma omp critical
                        cout << "I am thread " << i <<endl;
                    }
                }
                i--;
            }
        }
    }

    void thirteen_third(){
        int threads = 8;

#pragma omp parallel num_threads(threads)
        for (int i = threads - 1; i >= 0; i--) {
#pragma omp barrier
            if (i == omp_get_thread_num()) {
                printf("thread: %d\n", i);
            }
        }

}

    void thirteen_forth()
    {
    #pragma omp parallel num_threads(8)
    {
        Sleep(1000 / (omp_get_thread_num() + 1));
        printf("Thread %d \n", omp_get_thread_num());
    }
}
int thirteen_fifth() {
    int current_thread = 7;
#pragma omp parallel num_threads(8)
    {
        bool isDone = false;
        while (!isDone)
        {
#pragma omp critical
            {
                if (omp_get_thread_num() == current_thread)
                {
                    current_thread--;
                }
            }
            printf("thread = %d \n", omp_get_thread_num());
            isDone = true;
        }
    }
}
int main()
{
    thirteen_fifth();
	return EXIT_SUCCESS;
}