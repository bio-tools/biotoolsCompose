# Automatic-Workflow-Composition

Collection of material including data files, scripts, workflows and results for the proteomics use cases described in the publication "Automated workflow composition from a pool of bioinformatics tools: a demonstration for mass spectrometry data analysis" (in preparation).

The workflows of the four use cases are easily executed within the given Docker container. However, the bash scripts can also be adapted to run independently by adjusting the paths of the used software tools (if available on the respective computer). 

__Download files__    
_git clone https://github.com/veitveit/Automatic-Workflow-Composition_

__Create docker image:__    
_docker -t workflow-composition ._  
This will take a while.   
(you need to be in the folder Automatic-Workflow-Composition)

__Run workflows__   
E.g. Use case 3 workflow 1:   
_docker run -it workflow-composition_   
_cd UseCases/Use_case_3-PTM_identification/Workflow1_   
_bash FullPipeline_   
You can also run the R scripts with _R CMD BATCH GivenRScriptName_ and/or extract the files with _docker cp_
