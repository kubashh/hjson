# Benchmark HJson (0.4.0) vs cJSON (1.7.19). 18 Feb 2026

## x1
| path (size) | hjson | cjson | difference (%) |
| ----------- | ----- | ----- | -------------- |
| bool.json (5.05KB) | 49.45μs | 55.52μs | 12.29% |
| number.json (10.36KB) | 107.83μs | 139.96μs | 29.79% |
| string.json (9.73KB) | 53.78μs | 50.56μs | -5.97% |
| array.json (6.95KB) | 154.49μs | 247.17μs | 59.99% |
| object.json (6.17KB) | 40.09μs | 44.00μs | 9.76% |
| realCase.json (4.04KB) | 28.70μs | 33.59μs | 17.03% |
| realCase2.json (2.99KB) | 19.00μs | 22.91μs | 20.59% |
| realCase3.json (3.27KB) | 19.62μs | 22.49μs | 14.59% |
| realCase4-configs.json (9.96KB) | 60.06μs | 62.44μs | 3.95% |
| small.json (0.08KB) | 2.44μs | 2.38μs | -2.82% |
| big.json (138.69KB) | 646.24μs | 840.19μs | 30.01% |
| All (197.29KB) | 1.18ms | 1.52ms | 28.73% |
## x33
| path (size) | hjson | cjson | difference (%) |
| ----------- | ----- | ----- | -------------- |
| bool.json (5.05KB) | 828.46μs | 830.34μs | 0.23% |
| number.json (10.36KB) | 2.10ms | 3.03ms | 44.70% |
| string.json (9.73KB) | 1.14ms | 1.28ms | 12.65% |
| array.json (6.95KB) | 4.03ms | 4.87ms | 21.01% |
| object.json (6.17KB) | 892.57μs | 1.13ms | 26.57% |
| realCase.json (4.04KB) | 608.53μs | 774.54μs | 27.28% |
| realCase2.json (2.99KB) | 422.19μs | 501.25μs | 18.73% |
| realCase3.json (3.27KB) | 441.68μs | 514.31μs | 16.45% |
| realCase4-configs.json (9.96KB) | 1.45ms | 1.69ms | 16.35% |
| small.json (0.08KB) | 17.60μs | 22.14μs | 25.80% |
| big.json (138.69KB) | 18.17ms | 23.12ms | 27.27% |
| All (197.29KB) | 30.09ms | 37.77ms | 25.52% |
## x1000
| path (size) | hjson | cjson | difference (%) |
| ----------- | ----- | ----- | -------------- |
| bool.json (5.05KB) | 25.13ms | 23.89ms | -4.97% |
| number.json (10.36KB) | 64.46ms | 91.10ms | 41.33% |
| string.json (9.73KB) | 34.31ms | 36.48ms | 6.32% |
| array.json (6.95KB) | 122.82ms | 144.55ms | 17.69% |
| object.json (6.17KB) | 27.84ms | 33.45ms | 20.19% |
| realCase.json (4.04KB) | 18.86ms | 22.16ms | 17.48% |
| realCase2.json (2.99KB) | 12.57ms | 14.77ms | 17.43% |
| realCase3.json (3.27KB) | 13.06ms | 15.12ms | 15.77% |
| realCase4-configs.json (9.96KB) | 42.90ms | 49.66ms | 15.75% |
| small.json (0.08KB) | 499.85μs | 607.55μs | 21.55% |
| big.json (138.69KB) | 618.92ms | 724.35ms | 17.04% |
| All (197.29KB) | 981.37ms | 1.16s | 17.81% |
