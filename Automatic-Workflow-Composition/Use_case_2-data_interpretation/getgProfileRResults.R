## gProfileR 
library(gProfileR)
geneList <- read.csv("UniprotAccs.txt")
genes<-as.vector(geneList)
GP<-gprofiler(genes, organism="hsapiens", src_filter=c("KEGG"), min_set_size = 5, max_set_size = 500, correction_method="fdr")
write.csv(GP[order(GP$p.value),], file="gProfileRoutput.txt")

GP2<-gprofiler(genes, organism="hsapiens", src_filter=c("KEGG"), min_set_size = 5, max_set_size = 500, correction_method="fdr", png_fn="image.png")






