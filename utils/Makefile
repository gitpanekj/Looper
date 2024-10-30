SRC=del
.PHONY: ll, opt, dot, clean
PASSES=dse,instcombine,mem2reg

ll:
    clang -Xclang -disable-O0-optnone -fno-discard-value-names -S -emit-llvm $(SRC).c -o $(SRC).ll

opt: ll
    opt -S -passes="$(PASSES)" $(SRC).ll -o opt.ll


dot-cfg: opt
    opt -S -passes='dot-cfg' opt.ll -disable-output
    dot -Tpng -o cfg.png .bar.dot

dot-regions: opt
    opt -S --dot-regions-only opt.ll -disable-output
    dot -Tpng -o regions.png reg.foo.dot


cfg: dot-cfg
    gwenview cfg.png
    rm *.ll .bar.dot cfg.png

regions: dot-regions
    gwenview regions.png

clean:
    rm *.ll