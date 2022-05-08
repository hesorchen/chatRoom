
```cpp

#include <bits/stdc++.h>

using namespace std;

const int maxn = 55;

int mp[maxn][maxn];
int f[maxn << 1][maxn][maxn];

int main()
{
	int n, m;
	scanf("%d%d", &n, &m);
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
			scanf("%d", &mp[i][j]);

	for (int k = 2; k <= n + m; k++)
	{
		for (int i = 1; i < k; i++)
		{
			for (int j = 1; j < k; j++)
			{
				int &v = f[k][i][j];
				int tmp = mp[i][k - i] + mp[j][k - j];
				if (i == j)
					tmp -= mp[j][k - j];
				v = max(f[k - 1][i - 1][j - 1], v); // 1向下走，2向右走
				v = max(f[k - 1][i - 1][j], v);		// 1向下走，2向下走
				v = max(f[k - 1][i][j - 1], v);		// 1向右走，2向右走
				v = max(f[k - 1][i][j], v);			// 1向右走，2向下走
				v += tmp;
			}
		}
	}

	printf("%d\n", f[n + m][n][n]);
	return 0;
}
```