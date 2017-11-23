library(venneuler)
library(svglite)
library(XML)


W1 <- read.csv("Workflow1/iPRG2012_test_1_Default_PSM_Report.txt",sep="\t")

W2 <- xmlRoot(xmlTreeParse("Workflow2/CometOut.interact.ptm.ipro.pep.xml"))

## Filtering W2 
options(stringsAsFactors = FALSE)
# not working for multiple msms_run_summary, therefor new for loop
msms_indices <- which(names(W2) == "msms_run_summary")
PSMtab <- NULL
for (msrun in msms_indices) {
  msms_run_summary <- W2[[msrun]]
  sampleEnzyme <- xmlGetAttr(msms_run_summary[["sample_enzyme"]], 
                             "name")
  searchEngine <- xmlGetAttr(msms_run_summary[["search_summary"]], 
                             "search_engine")
  searchDB <- msms_run_summary[["search_summary"]][["search_database"]]
  n <- which(names(msms_run_summary) == "spectrum_query")
  spqrieslist <- msms_run_summary[n]
  spectrumQueries <- lapply(spqrieslist, function(spqu) {
    attrs <- xmlAttrs(spqu)
    if (xmlSize(spqu) > 0) {
      hit <- xmlApply(spqu[[1]], xmlAttrs)
      hitTab <- do.call(rbind, hit)
      if(sum(colnames(hitTab) == "protein_descr") == 0) hitTab <- cbind(hitTab,protein_descr=NA)
      # print(colnames(hitTab))
      # print(length(hitTab))
      tmp <- xmlApply(spqu[[1]], function(x) {
        scores <- xmlSApply(x, xmlGetAttr, "value")
        scorenames <- xmlSApply(x, xmlGetAttr, "name")
        idx <- which(names(scores) == "search_score")
        scoreslist <- unlist(scores[idx])
        names(scoreslist) <- unlist(scorenames[idx])
        idxx <- which(names(scores) == "modification_info")
        if (length(idxx) > 0) {
          modification <- as.data.frame(xmlSApply(x[["modification_info"]],
                                                  xmlAttrs))
          modseq <- xmlGetAttr(x[["modification_info"]],"modified_peptide")
        }
        else {
          modification <- NA
          modseq <- NA
        }
        analysislist <- xmlSApply(x, xmlGetAttr, "analysis")
        idxxx <- which(names(analysislist) == "analysis_result")
        probs <- xmlElementsByTagName(x,"analysis_result")
        probsnames <- unlist(lapply(probs,xmlGetAttr,"analysis"))
        # print(x)
        if (sum(probsnames == "peptideprophet") > 0) {
          probslist <- as.numeric(xmlSApply(probs[[which(probsnames == "peptideprophet")]],xmlGetAttr,"probability"))
        } else {
          probslist <- NA
        }
        if (sum(probsnames == "interprophet") > 0) {
          probslist <- append(probslist,as.numeric(xmlSApply(probs[[which(probsnames == "interprophet")]],xmlGetAttr,"probability")))
        } else {
          probslist <- append(probslist,NA)
        }
        
        if (sum(probsnames == "ptmprophet") > 0) {
          ptmlist <- xmlApply(probs[[which(probsnames == "ptmprophet")]],function(x) x )
          ptmlist <- (lapply(ptmlist, function(x) {xmlElementsByTagName(x, "mod_aminoacid_probability")}))
          ptmlist <- as.numeric(unlist(lapply(ptmlist,function(x) lapply(x, xmlGetAttr, "probability"))))
          if (min(ptmlist) == 1) 
            ptmlist <- 1
          else
           ptmlist <- max(ptmlist)-min(ptmlist)
        } else {
          ptmlist <- NA
        }
        res <- c(scoreslist, probslist, ptmlist, modification = paste(unlist(modification), 
                                                                      collapse = ";"),modseq)
        names(res) <- c(names(scoreslist),"peptideprophet","interprophet","ptmprophet","modification","W2Seqs")
        res
      })
      PSM <- cbind(hitTab, do.call(rbind, tmp))
      # print(hitTab)
      if (!is.null(PSM)) {
        PSM <- t(apply(PSM, 1, function(x) c(attrs, x)))
      }
    }
  })
  PSMtab <- rbind(PSMtab, do.call(rbind.data.frame, spectrumQueries))
}
rownames(PSMtab) <- NULL
##### from function pepXMLTab() in pepXMLTab package

W2 <- PSMtab

W2 <- W2[!is.na(W2$W2Seqs),]
W2$W2Seqs <- gsub("S\\[167\\]","S<p>",W2$W2Seqs)
W2$W2Seqs <- gsub("T\\[181\\]","T<p>",W2$W2Seqs)
W2$W2Seqs <- gsub("Y\\[243\\]","Y<p>",W2$W2Seqs)
W2$W2Seqs <- gsub("K\\[170\\]","K<ace>",W2$W2Seqs)
W2$W2Seqs <- gsub("M\\[147\\]","M<ox>",W2$W2Seqs)
W2$W2Seqs <- gsub("N\\[115\\]","N<deam>",W2$W2Seqs)
W2$W2Seqs <- gsub("Q\\[129\\]","Q<deam>",W2$W2Seqs)
W2$W2Seqs <- gsub("K\\[142\\]","K<meth>",W2$W2Seqs)
W2$W2Seqs <- gsub("K\\[156\\]","K<dimeth>",W2$W2Seqs)
W2$W2Seqs <- gsub("R\\[170\\]","R<meth>",W2$W2Seqs)
W2$W2Seqs <- gsub("R\\[184\\]","R<dimeth>",W2$W2Seqs)

W2$W2Seqs <- gsub("\\[[0-9]* \\]","",W2$W2Seqs)
W2$W2Seqs <- gsub("n","",W2$W2Seqs)
W2$protein <- sub("\\|.*","",sub("[a-z]*\\|","",W2$protein))

# Confidence == 100 
FilteredW1 <- W1[W1$Confidence.... > 90 & 
                   !grepl("Random",W1$Localization.Confidence) &
                   !grepl("Not Scored",W1$Localization.Confidence),]
FilteredW1$Modified.Sequence <- sub("NH2-","",sub("-COOH","",FilteredW1$Modified.Sequence))
FilteredW1 <- FilteredW1[!is.na(FilteredW1$Confidence....),]
length(unique(FilteredW1$Modified.Sequence))

# Only modified
W1Unmod <- FilteredW1[FilteredW1$Localization.Confidence == "",]
FilteredW1 <- FilteredW1[FilteredW1$Localization.Confidence != "",]



## Filtering: ptmprophet > 0.9 , no peptideprophet, xcorr > 1.5, phosphorylation
FilteredW2 <- W2[as.numeric(W2$ptmprophet) > 0.9 & as.numeric(W2$interprophet) > 0.9,]
W2Unmod <- W2[as.numeric(W2$interprophet) > 0.9 & !grepl(">",W2$W2Seqs),]
FilteredW2 <- FilteredW2[!is.na(FilteredW2$W2Seqs),]


# FilteredW2 <- W2[!grepl("m",W2$W2Seqs) & W2$globalFLR > 0.9 & W2$pepProphet > 0.9 & 
#                    W2$pep1score > 1,]
# FilteredW2 <- W2[!grepl("m",W2$predictedPep1) & W2$globalFLR > 0.9 & W2$pepProphet > 0.9 & 
#                    W2$pep1score > 1,]
# FilteredW2 <- W2
length(unique(FilteredW2$W2Seqs))

# ## Filtering original data
# # 
# FilteredOrig <- Orig[Orig$Ionscore > 50 & Orig$Phosphosite.Position != "" & Orig$Mascot.Delta.Score > 20,]
# # Generating peptide sequences
# FilteredOrig <- cbind(FilteredOrig,PhSeqs=NA)
# for (i in 1:nrow(FilteredOrig)) {
#   poss <- as.numeric(unlist(strsplit(as.character(FilteredOrig[i,"Phosphosite.Position"]),",")))
#   tstr <- as.character(FilteredOrig[i,"Peptide.Sequence"])
#   add <- 0
#   for (p in poss) {
#     tstr <- paste(substr(tstr, 1, p+add), "<p>", substr(tstr, p+add+1, nchar(tstr)), sep = "")
#     add <- add + 3
#   }
#   FilteredOrig[i,"PhSeqs"] <- tstr
# }
# length(unique(FilteredOrig$PhSeqs))


## Load synthetic peptides
W3 <- read.csv("SynthPeps.csv")
W3[1:5,1] <- gsub("k","K<ace>",W3[1:5,1])
W3[6:10,1] <- gsub("k","K<dimeth>",W3[6:10,1])
W3[11:16,1] <- gsub("r","R<dimeth>",W3[11:16,1])
W3[17:21,1] <- gsub("k","K<meth>",W3[17:21,1])
W3[22:26,1] <- gsub("r","R<meth>",W3[22:26,1])
W3[32:60,1] <- gsub("s","S<p>",W3[32:60,1])
W3[32:60,1] <- gsub("t","T<p>",W3[32:60,1])
W3[32:60,1] <- gsub("y","Y<p>",W3[32:60,1])
W3[61:65,1] <- gsub("y","Y<s>",W3[61:65,1])
W3[66:70,1] <- gsub("k","K<trimeth>",W3[66:70,1])


## Compare accession numbers
# svglite("iPRG2012WorkflowComparison.svg",height=6,width=12)
par(mfrow=c(1,2))
protsW1 <- unique(FilteredW1$Protein.s.)
protsW2 <- unique(FilteredW2$protein)
# protsW3 <- unique(FilteredOrig$Protein.ID)
# venn <- venneuler(rbind(cbind(as.character(protsW1),"Workflow 1"),cbind(as.character(protsW2),"Workflow 2"),cbind(as.character(protsW3),"Original data")))
# venn$labels <- paste(venn$labels,c(length(protsW1),length(protsW2),length(protsW3)),sep="\n")
# venn$labels <- paste(venn$labels," (",c(table(sapply(protsW1, nchar)<4)["TRUE"],table(sapply(protsW2, nchar)<4)["TRUE"],
#                                         table(sapply(protsW3, nchar)<4)["TRUE"]),")",sep="")
venn <- venneuler(rbind(cbind(as.character(protsW1),"Workflow 1"),cbind(as.character(protsW2),"Workflow 2")))
venn$labels <- paste(venn$labels,c(length(protsW1),length(protsW2)),sep="\n")
 venn$labels <- paste(venn$labels," (",c(table(sapply(protsW1, nchar)<4)["TRUE"],table(sapply(protsW2, nchar)<4)["TRUE"]),")",sep="")
plot(venn,main="Common proteins")
intersect(protsW1,protsW2)
# intersect(protsW1,intersect(protsW2,protsW3))

pepsW1 <- unique(FilteredW1$Modified.Sequence)
pepsW2 <- unique(FilteredW2$W2Seqs)
pepsW3 <- W3[,1]
 venn <- venneuler(rbind(cbind(as.character(pepsW1),"Workflow 1"),cbind(as.character(pepsW2),"Workflow 2"),cbind(as.character(pepsW3),"Original data")))
 venn$labels <- paste(venn$labels,c(length(pepsW1),length(pepsW2),length(pepsW3)),sep="\n")
venn <- venneuler(rbind(cbind(as.character(pepsW1),"Workflow 1"),cbind(as.character(pepsW2),"Workflow 2"),
                        cbind(as.character(pepsW3),"Synthetic peptides")))
venn$labels <- paste(venn$labels,c(length(pepsW1),length(pepsW2),length(pepsW3)),sep="\n")

plot(venn,main="Common modified peptides")
commonPeps <- intersect(pepsW1,pepsW2)
sort(intersect(pepsW1,pepsW3))
sort(intersect(pepsW2,pepsW3))
sort(intersect(pepsW1,pepsW2))
intersect(pepsW1,intersect(pepsW2,pepsW3))
# protsWithCommonPeps <- sort(unique(FilteredW2[FilteredW2$predictedPep1 %in% commonPeps,1]))
# dev.off()
par(mfrow=c(1,1))



## table of found PTMs
foundModTypes <- rbind(c(length(grep("p",pepsW1)),length(grep("p",pepsW2)),length(grep("p",pepsW3))),
                       c(length(grep("ace",pepsW1)),length(grep("ace",pepsW2)),length(grep("ace",pepsW3))),
                       c(length(grep("<meth",pepsW1)),length(grep("<meth",pepsW2)),length(grep("<meth",pepsW3))),
                       c(length(grep("<dimeth",pepsW1)),length(grep("<dimeth",pepsW2)),length(grep("dimeth",pepsW3))),
                       c(length(grep("trimeth",pepsW1)),length(grep("trimeth",pepsW2)),length(grep("trimeth",pepsW3))),
                       c(length(grep("<s>",pepsW1)),length(grep("s",pepsW2)),length(grep("s",pepsW3))),
                       c(length(grep("<ox>",pepsW1)),length(grep("ox",pepsW2)),length(grep("ox",pepsW3))))
colnames(foundModTypes) <- c("Workflow 1","Workflow 2", "Synthetic peptides")
rownames(foundModTypes) <- c("Phosphorylation", "Acetylation", "Methylation","Dimethylation", "Trimethlation", "Sulfonation", "Oxidation")
barplot(t(foundModTypes[,1:2]),horiz = T,beside = T,las=T,legend.text = colnames(foundModTypes)[1:2],border=F,col=c("#AB5532","#13889B"))


# Compare unmodified peptides
intersect(as.character(W1Unmod$Protein.s.), as.character(W2Unmod$protein))
intersect(as.character(W1Unmod$Sequence), as.character(W2Unmod$W2Seqs))
