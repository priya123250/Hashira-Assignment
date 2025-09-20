#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <jansson.h>

uint64_t convert_to_decimal(const char *num, int base) {
    uint64_t decimal = 0;
    int len = strlen(num);
    for (int i = 0; i < len; i++) {
        char c = num[i];
        int digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'z') digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'Z') digit = c - 'A' + 10;
        else { printf("Invalid character
"); exit(1);}
        decimal = decimal * base + digit;
    }
    return decimal;
}

uint64_t mod_inverse(uint64_t a, uint64_t m) {
    uint64_t m0 = m, y = 0, x = 1;
    if (m == 1) return 0;
    while (a > 1) {
        uint64_t q = a / m, t = m;
        m = a % m; a = t; t = y;
        y = x - q * y; x = t;
    }
    if ((int64_t)x < 0) x += m0;
    return x;
}

uint64_t calculate_secret(uint64_t *x_values, uint64_t *y_values, int k) {
    uint64_t secret = 0;
    uint64_t prime = (1ULL << 61) - 1;
    for (int i = 0; i < k; i++) {
        uint64_t numerator = 1, denominator = 1;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                numerator = (numerator * ((prime - x_values[j]) % prime)) % prime;
                denominator = (denominator * ((x_values[i] - x_values[j] + prime) % prime)) % prime;
            }
        }
        uint64_t term = (y_values[i] * numerator % prime * mod_inverse(denominator, prime)) % prime;
        secret = (secret + term) % prime;
    }
    return secret;
}

int main() {
    json_error_t error;
    json_t *root = json_load_file("input.json", 0, &error);
    if (!root) { fprintf(stderr, "error: %s
", error.text); return 1; }

    size_t index;
    json_t *test_case;
    json_array_foreach(root, index, test_case) {
        int k = json_integer_value(json_object_get(test_case, "k"));
        json_t *x_array = json_object_get(test_case, "x");
        json_t *y_array = json_object_get(test_case, "y");
        json_t *base_array = json_object_get(test_case, "base");

        uint64_t x_values[k], y_values[k];
        for (int i = 0; i < k; i++) {
            x_values[i] = json_integer_value(json_array_get(x_array, i));

            const char *y_str = json_string_value(json_array_get(y_array, i));
            int base = json_integer_value(json_array_get(base_array, i));
            y_values[i] = convert_to_decimal(y_str, base);
        }

        uint64_t secret = calculate_secret(x_values, y_values, k);
        printf("%llu
", secret); // Only print 'c' value
    }

    json_decref(root);
    return 0;
}