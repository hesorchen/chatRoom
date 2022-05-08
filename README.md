> test

#### 模板<!--more-->

```cpp
#include <bits/stdc++.h>
using namespace std;

const int N = 1e5 + 10;
const int M = 350;

long long sum[M], add[M], w[N];
int len;
int n, m;

int get(int i)
{
    return (i - 1) / len + 1;
}

void ADD(int l, int r, int val)
{
    if (get(l) == get(r))
    {
        for (int i = l; i <= r; i++)
            w[i] += val, sum[get(i)] += val;
        return;
    }
    int i = l, j = r;
    while (get(i) == get(l))
        sum[get(i)] += val, w[i] += val, ++i;
    while (get(j) == get(r))
        sum[get(j)] += val, w[j] += val, --j;
    for (int k = get(i); k <= get(j); k++)
        sum[k] += 1ll * val * len, add[k] += val;
}
long long SUM(int l, int r)
{
    long long res = 0;
    if (get(l) == get(r))
    {
        for (int i = l; i <= r; i++)
            res += w[i] + add[get(i)];
        return res;
    }
    int i = l, j = r;
    while (get(i) == get(l))
        res += w[i] + add[get(i)], ++i;
    while (get(j) == get(r))
        res += w[j] + add[get(j)], --j;
    for (int k = get(i); k <= get(j); k++)
        res += sum[k];
    return res;
}

int main()
{
    scanf("%d %d", &n, &m);
    len = sqrt(n);
    for (int i = 1; i <= n; i++)
    {
        int temp;
        scanf("%d", &temp);
        w[i] += temp;
        sum[get(i)] += temp;
    }
    while (m--)
    {
        int op;
        scanf("%d", &op);
        if (op == 1)
        {
            int x, y, k;
            scanf("%d %d %d", &x, &y, &k);
            ADD(x, y, k);
        }
        else
        {
            int x, y;
            scanf("%d %d", &x, &y);
            printf("%lld\n", SUM(x, y));
        }
    }
    return 0;
}
```