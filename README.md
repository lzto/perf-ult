custom perf userland tool to use PEBS/PT with AUX buffer

* have to be used with v4.5-ipv10+ kernel

```shell
make
./rud
setup_mmap: fd 3
mmap m=0x7faed2b0b000 + 528384
mmap aux_pt=0x7faed2a8b000, aux_pebs=0x7faed2acb000
...
TSC:0x815b9253947910, IP:0x7faed270f0c8
TSC:0x815b92539490e0, IP:0x7faed270f0c8
TSC:0x815b925394b116, IP:0x7faed2660f70
TSC:0x815b925394d037, IP:0x7faed265b1f8
TSC:0x815b925395b735, IP:0x7faed264782c
TSC:0x815b925395db79, IP:0x400d3a
TSC:0x815b925395f242, IP:0x400d3a
TSC:0x815b9253960957, IP:0x400d3a
TSC:0x815b925396201c, IP:0x400d3a
TSC:0x815b9253963744, IP:0x400d3a
TSC:0x815b9253964e59, IP:0x400d3a
TSC:0x815b925396651e, IP:0x400d3a
...............
```

