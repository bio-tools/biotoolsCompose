package biotools.biotoolsCompose.poc.domainSetup;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import org.semanticweb.owlapi.apibinding.OWLManager;
import org.semanticweb.owlapi.model.IRI;
import org.semanticweb.owlapi.model.OWLAnnotation;
import org.semanticweb.owlapi.model.OWLAnnotationValue;
import org.semanticweb.owlapi.model.OWLClass;
import org.semanticweb.owlapi.model.OWLDataFactory;
import org.semanticweb.owlapi.model.OWLLiteral;
import org.semanticweb.owlapi.model.OWLOntology;
import org.semanticweb.owlapi.model.OWLOntologyCreationException;
import org.semanticweb.owlapi.model.OWLOntologyManager;
import org.semanticweb.owlapi.reasoner.Node;
import org.semanticweb.owlapi.reasoner.NodeSet;
import org.semanticweb.owlapi.reasoner.OWLReasoner;
import org.semanticweb.owlapi.reasoner.structural.StructuralReasonerFactory;
import org.semanticweb.owlapi.search.EntitySearcher;

import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.io.xml.DomDriver;

import de.jabc.plugin.ontEDAPI.OntEDAPI;
import de.jabc.plugin.ontEDAPI.OntEDFactory;
import de.jabc.plugin.ontEDAPI.Exceptions.OntEDException;
import de.jabc.plugin.ontEDAPI.Exceptions.OntEDMissingImportException;
import de.jabc.plugin.ontEDAPI.defaultImpl.DefaultFactory;
import de.jabc.plugin.ontEDAPI.ontEDInterfaces.OntEDClass;
import de.jabc.plugin.ontEDAPI.ontEDInterfaces.OntEDDataProperty;
import de.jabc.plugin.ontEDAPI.ontEDInterfaces.OntEDIndividual;
import de.jabc.plugin.prophets.lib.data.SynthesisModuleSIB;
import de.jabc.plugin.prophets.lib.defaultImpl.XStreamModuleProvider;

public class DomainSetup {

	// this URL always points to the last stable version
	private static String edamurl = "http://edamontology.org/EDAM.owl";

	// fields to provide easy access to EDAM for all methods in this class
	private static OWLOntology edamOntology;
	private static OWLOntologyManager ontManager;
	private static OWLDataFactory edamDataFactory;

	public static void main(String[] args) throws OntEDException, IOException, OntEDMissingImportException {
		// set path to EDAM, CSV file with annotations, PROPHETS project
		// directory
		// (TODO: read local paths from command line)

		String projectdir = args[0];
		String csvfile = args[1];

		// load EDAM
		IRI edamIRI = IRI.create(edamurl);
		ontManager = OWLManager.createOWLOntologyManager();
		try {
			edamOntology = ontManager.loadOntologyFromOntologyDocument(edamIRI);
			// System.out.println("Loaded EDAM ontology.");
		} catch (OWLOntologyCreationException e) {
			System.err.println("Error loading EDAM ontology.");
			e.printStackTrace();
			return;
		}
		edamDataFactory = ontManager.getOWLDataFactory();

		// run EDAM2Onted
		EDAM2PROPHETS(projectdir);
		System.out.println("Import of EDAM ontology complete.");

		// create modules.xml based on CSV file, extend taxonomies accordingly
		CSV2PROPHETS(csvfile, projectdir);
		System.out.println("Import of module annotations complete.");

		// optional but useful: prune taxonomies to the actually used terms
		shrinkTaxonomies(projectdir);
		System.out.println("Taxonomy pruning copmlete.");

		System.out.println("=> Generation of PROPHETS domain model complete.");
	}

	/**
	 * Creates PROPHETS module and type taxonomies from EDAM's operation and
	 * format branches, respectively
	 * 
	 * @param projectdir
	 * @throws OntEDException
	 * @throws FileNotFoundException
	 */
	private static void EDAM2PROPHETS(String projectdir) throws OntEDException, FileNotFoundException {
		File servicetaxonomyfile = new File(projectdir + "/prophets/moduleTaxonomy.owl");
		File typetaxonomyfile = new File(projectdir + "/prophets/typeTaxonomy.owl");

		/* CREATE PROPHETS-COMPATIBLE MODULE TAXONOMY (using OntED) */

		// create empty ontology
		OntEDFactory factory_modules = new DefaultFactory();
		OntEDAPI api_modules;
		api_modules = new OntEDAPI(factory_modules);
		api_modules.createNewOntology(URI.create("http://de/jabc/prophets/modules"));

		// add data properties for sibUID and branchName
		OntEDDataProperty sibUID = factory_modules
				.instantiateOntEDDataProperty(URI.create("http://de/jabc/prophets/modules/properties/sibUID"));
		api_modules.createDataProperty(sibUID);
		OntEDDataProperty branchName = factory_modules
				.instantiateOntEDDataProperty(URI.create("http://de/jabc/prophets/modules/properties/branchName"));
		api_modules.createDataProperty(branchName);

		// get "operation" class, http://edamontology.org/operation_0004
		OWLClass operationClass = edamDataFactory.getOWLClass(IRI.create("http://edamontology.org/operation_0004"));

		// add operation terms from EDAM to service taxonomy
		addClassTree2OntedOntology(operationClass, api_modules.getOwlThing(), factory_modules, api_modules,
				edamDataFactory, edamOntology);

		// save module taxonomy
		api_modules.writeOntologyToOutputStream(new FileOutputStream(servicetaxonomyfile), "OWLXML");

		/* CREATE PROPHETS-COMPATIBLE TYPE TAXONOMY (using OntED) */

		// create empty ontology
		factory_modules = new DefaultFactory();
		api_modules = new OntEDAPI(factory_modules);
		api_modules.createNewOntology(URI.create("http://de/jabc/prophets/types"));

		// add data properties for sibUID and branchName
		sibUID = factory_modules
				.instantiateOntEDDataProperty(URI.create("http://de/jabc/prophets/types/properties/sibUID"));
		api_modules.createDataProperty(sibUID);
		branchName = factory_modules
				.instantiateOntEDDataProperty(URI.create("http://de/jabc/prophets/types/properties/branchName"));
		api_modules.createDataProperty(branchName);

		// get "format" class, http://edamontology.org/format_1915
		OWLClass formatClass = edamDataFactory.getOWLClass(IRI.create("http://edamontology.org/format_1915"));

		// add operation terms from EDAM to service taxonomy
		addClassTree2OntedOntology(formatClass, api_modules.getOwlThing(), factory_modules, api_modules,
				edamDataFactory, edamOntology);

		// save module taxonomy
		api_modules.writeOntologyToOutputStream(new FileOutputStream(typetaxonomyfile), "OWLXML");

	}

	/**
	 * recursive method for walking through a class tree and turn it into an
	 * OntED ontology (for use in PROPHETS)
	 *
	 * @param edamClass
	 * @param ontedsuperclass
	 * @param ontedfactory
	 * @param ontedapi
	 * @param edamDataFactory
	 * @param edamOntology
	 * @throws OntEDException
	 */
	private static void addClassTree2OntedOntology(OWLClass edamClass, OntEDClass ontedsuperclass,
			OntEDFactory ontedfactory, OntEDAPI ontedapi, OWLDataFactory edamDataFactory, OWLOntology edamOntology)
					throws OntEDException {

		// add the (new) class to the OntED ontology
		URI classURI = URI.create(ontedapi.getOntologyURI() + "#" + getLabel(edamClass));
		OntEDClass ontedclass = null;

		try {
			// This seems to be the only way of checking if the
			// individual exists.
			ontedclass = ontedapi.getOntEDClass(classURI);
		} catch (OntEDException e) {
			// TODO: do I really want to check for the string message?
			// but if not,
			// what happens when some other onted exception occurs?
			if (e.getMessage().equals("OntEDClass object " + classURI.toASCIIString() + " does not exist!")) {
				ontedclass = ontedfactory.instantiateOntEDClass(classURI);
				ontedapi.createClass(ontedclass);
			}
		}

		// add a subclass relationship between the class and its superclass
		ontedapi.addSubClassOfAxiom(ontedsuperclass, ontedclass);
		// System.out.println("Added relationship: " + ontedclass + " is_a " +
		// ontedsuperclass);

		// add the classe's subclasses recursively
		OWLReasoner r = new StructuralReasonerFactory().createReasoner(edamOntology);
		NodeSet<OWLClass> subclasses = r.getSubClasses(edamClass, true);

		for (Node<OWLClass> subClass : subclasses) {
			// if (!(subClass.getRepresentativeElement().isOWLNothing())) {
			addClassTree2OntedOntology(subClass.getRepresentativeElement(), ontedclass, ontedfactory, ontedapi,
					edamDataFactory, edamOntology);
			// }
		}

	}

	/**
	 * returns the label of an OWL/EDAM class, replaces whitespaces (which OntEd
	 * can not handle) with "_"
	 * 
	 * @param owlclass
	 * @param owlontology
	 * @param owldatafactory
	 * @return
	 */
	private static String getLabel(OWLClass edamclass) {

		if (!(edamclass.isOWLNothing())) {
			for (OWLAnnotation a : EntitySearcher.getAnnotations(edamclass, edamOntology,
					edamDataFactory.getRDFSLabel())) {
				OWLAnnotationValue val = a.getValue();
				if (val instanceof OWLLiteral) {
					String label = ((OWLLiteral) val).getLiteral();
					label = label.replace(" ", "_");
					return label;
				}
			}
		} else {
			return edamclass.toString();
		}
		return "N/A";
	}

	/**
	 * Reads the tool annotations from a CSV file, creates the modules.xml and
	 * extends the taxonomies accordingly
	 * 
	 * @param csvfile
	 * @param projectdir
	 * @throws IOException
	 * @throws OntEDException
	 * @throws OntEDMissingImportException
	 */
	private static void CSV2PROPHETS(String csvfile, String projectdir)
			throws IOException, OntEDException, OntEDMissingImportException {

		// create a set for collecting all module definitions
		Set<SynthesisModuleSIB> modules = new HashSet<SynthesisModuleSIB>();

		// load module taxonomy
		OntEDFactory ontedfactory = new DefaultFactory();
		OntEDAPI ontedapi = new OntEDAPI(ontedfactory);
		ontedapi.loadOntology(new File(projectdir + "/prophets/moduleTaxonomy.owl"));

		// read csv table that contains the annotations of the ms-utils
		BufferedReader br = new BufferedReader(new FileReader(csvfile));
		String line = "";
		String separator = ",";

		// get headline (column titles) and indexes of relevant columns
		String[] columns = br.readLine().split(separator);
		int indexOfName = getIndexOf(columns, "name");
		int indexOfOperations = getIndexOf(columns, "EDAM operation");
		int indexOfInFormats = getIndexOf(columns, "EDAM data format in");
		int indexOfOutFormats = getIndexOf(columns, "EDAM data format out");

		// get annotations for all tools
		ArrayList<String[]> tools = new ArrayList<String[]>();
		while ((line = br.readLine()) != null) {
			String[] tool = line.split(separator);
			tools.add(tool);
		}

		br.close();

		// for each tool...
		for (String[] tool : tools) {

			// collect relevant information
			String toolname = tool[indexOfName];
			String[] operations = tool[indexOfOperations].split("\\|");
			String[] inFormats = tool[indexOfInFormats].split("\\|");
			String[] outFormats = tool[indexOfOutFormats].split("\\|");
			int combinations = inFormats.length * outFormats.length;
			int counter = 1;

			// for each input/output combination...
			for (int i = 0; i < inFormats.length; i++) {
				for (int j = 0; j < outFormats.length; j++) {

					// create synthesis module for current tool
					String displayname = toolname;
					if (combinations > 1) {
						displayname = displayname + "_" + counter;
						counter++;
					}
					SynthesisModuleSIB sms = new SynthesisModuleSIB("biotoolsCompose/demo-sibs/" + toolname, "default",
							displayname);

					if (!inFormats[i].equals("")) {
						OWLClass inClass = edamDataFactory
								.getOWLClass(IRI.create("http://edamontology.org/" + inFormats[i]));
						sms.addInputType(getLabel(inClass));
					}
					if (!outFormats[j].equals("")) {
						OWLClass outClass = edamDataFactory
								.getOWLClass(IRI.create("http://edamontology.org/" + outFormats[j]));
						sms.addOutputType(getLabel(outClass));
					}

					modules.add(sms);

					// add the tool as (new) individual to the OntED ontology
					URI classURI = URI.create(ontedapi.getOntologyURI() + "#" + displayname);
					OntEDIndividual ontedindiviual = ontedfactory.instantiateOntEDIndividual(classURI);
					ontedapi.createIndividual(ontedindiviual);

					// add subclass relations according to operations
					for (String operation : operations) {
						if (!operation.equals("")) {
							OWLClass operationClass = edamDataFactory
									.getOWLClass(IRI.create("http://edamontology.org/" + operation));
							String ontedUri = "http://de/jabc/prophets/modules#" + getLabel(operationClass);
							OntEDClass ontedclass = ontedapi.getOntEDClass(URI.create(ontedUri));
							ontedapi.addTypeOfAxiom(ontedclass, ontedindiviual);
						}
					}

					// (input/output formats should already be in the taxonomy.
					// But maybe add is-a relation for the tool's parameters?

				}

			}

		}

		// write modules.xml
		XStream xs = new XStream(new DomDriver());
		xs.alias("module", SynthesisModuleSIB.class);
		String modulesxml = xs.toXML(modules);
		Writer writer = new BufferedWriter(new FileWriter(new File(projectdir + "/prophets/modules.xml")));
		writer.write(modulesxml);
		writer.close();

		// save module taxonomy
		ontedapi.writeOntologyToOutputStream(
				new FileOutputStream(new File(projectdir + "/prophets/moduleTaxonomy.owl")), "OWLXML");

	}

	/**
	 * small helper method that returns the (first) index of a string in a
	 * string array
	 * 
	 * @param array
	 * @param s
	 * @return
	 */
	private static int getIndexOf(String[] array, String s) {
		for (int i = 0; i < array.length; i++) {
			if (array[i].equals(s)) {
				return i;
			}
		}
		return -1;
	}
	
	/**
	 * Shrinks the taxonomies to the individuals and classes (and their
	 * transitive parents) that are contained in the modules.xml and thus
	 * relevant for the domain model.
	 * 
	 * @param projectdir
	 * @throws FileNotFoundException
	 * @throws OntEDException
	 * @throws OntEDMissingImportException
	 */
	private static void shrinkTaxonomies(String projectdir)
			throws FileNotFoundException, OntEDException,
			OntEDMissingImportException {
		
		String moduletaxonomyfile = projectdir + "/prophets/moduleTaxonomy.owl";
		String typetaxonomyfile = projectdir + "/prophets/typeTaxonomy.owl";
		String modulesxmlpath = projectdir + "/prophets/modules.xml";
		
		// strategy: mark/remember all nodes that are mentioned in the module
		// definitions and
		// their ancestors on the way to the root, then remove all other nodes
		// load modules.xml, get all used services and types
		XStreamModuleProvider moduleprovider = new XStreamModuleProvider(
				new File(modulesxmlpath));
		Set<SynthesisModuleSIB> modules = moduleprovider.getModules();

		Set<String> services = new HashSet<String>();
		Set<String> types = new HashSet<String>();

		for (SynthesisModuleSIB module : modules) {

			services.add(module.getDisplayName());

			Set<String> intypes = module.getInputTypes();
			Set<String> outtypes = module.getOutputTypes();
			Set<String> killedtypes = module.getKilledTypes();

			types.addAll(intypes);
			types.addAll(outtypes);
			types.addAll(killedtypes);
		}

		// load module taxonomy
		OntEDFactory ontedfactory = new DefaultFactory();
		OntEDAPI ontedapi = new OntEDAPI(ontedfactory);
		ontedapi.loadOntology(new File(moduletaxonomyfile));

		// prepare collections of nodes to be kept
		Set<OntEDIndividual> individualstokeep = new HashSet<OntEDIndividual>();
		Set<OntEDClass> classestokeep = new HashSet<OntEDClass>();

		// for all services:
		for (String service : services) {

			// keep individual
			URI nodeURI = URI.create(ontedapi.getOntologyURI() + "#" + service);
			OntEDIndividual individual = ontedapi.getOntEDIndividual(nodeURI);

			individualstokeep.add(individual);

			// identify and keep all superclasses
			Set<OntEDClass> alltypes = ontedapi.infGetTypes(individual);
			classestokeep.addAll(alltypes);
		}
		
		int moduleclasseskept = classestokeep.size();
		
		// remove the individuals that do not have to be kept
		Set<OntEDIndividual> individualstoremove = ontedapi
				.getOntEDIndividuals();
		individualstoremove.removeAll(individualstokeep);

		for (OntEDIndividual individualtoremove : individualstoremove) {
			ontedapi.removeIndividual(individualtoremove);
		}

		// remove the classes that do not have to be kept
		Set<OntEDClass> classestoremove = ontedapi.getOntEDClasses();
		classestoremove.removeAll(classestokeep);

		for (OntEDClass classtoremove : classestoremove) {
			ontedapi.removeOntEDClass(classtoremove);
		}

		// save module taxonomy
		ontedapi.writeOntologyToOutputStream(new FileOutputStream(new File(
				moduletaxonomyfile)), "OWLXML");

		// load type taxonomy
		ontedfactory = new DefaultFactory();
		ontedapi = new OntEDAPI(ontedfactory);
		ontedapi.loadOntology(new File(typetaxonomyfile));

		// prepare collections of nodes to be kept
		individualstokeep = new HashSet<OntEDIndividual>();
		classestokeep = new HashSet<OntEDClass>();

		// for all types:
		for (String type : types) {

			URI nodeURI = URI.create(ontedapi.getOntologyURI() + "#" + type);

			OntEDIndividual ontedindividual = null;
			OntEDClass ontedclass = null;

			try {
				// This seems to be the only way of checking
				// if the individual exists.
				ontedindividual = ontedapi.getOntEDIndividual(nodeURI);
			} catch (OntEDException e) {
			}

			try {
				// This seems to be the only way of checking
				// if the individual exists.
				ontedclass = ontedapi.getOntEDClass(nodeURI);
			} catch (OntEDException e) {
			}

			if (ontedindividual != null) {
				// keep individual
				individualstokeep.add(ontedindividual);

				// identify and keep all superclasses
				Set<OntEDClass> alltypes = ontedapi
						.infGetTypes(ontedindividual);
				classestokeep.addAll(alltypes);
			}
			if (ontedclass != null) {
				// keep class
				classestokeep.add(ontedclass);

				// identify and keep all superclasses
				Set<OntEDClass> allsuperclasses = ontedapi
						.infGetSuperClasses(ontedclass);
				classestokeep.addAll(allsuperclasses);
			}

		}

		int typeclasseskept = classestokeep.size();
		
		// remove the individuals that do not have to be kept
		individualstoremove = ontedapi.getOntEDIndividuals();
		individualstoremove.removeAll(individualstokeep);

		for (OntEDIndividual individualtoremove : individualstoremove) {
			ontedapi.removeIndividual(individualtoremove);
		}

		// remove the classes that do not have to be kept
		classestoremove = ontedapi.getOntEDClasses();
		classestoremove.removeAll(classestokeep);

		for (OntEDClass classtoremove : classestoremove) {
			ontedapi.removeOntEDClass(classtoremove);
		}

		// save module taxonomy
		ontedapi.writeOntologyToOutputStream(new FileOutputStream(new File(
				typetaxonomyfile)), "OWLXML");
		

		System.out.println(moduleclasseskept + " module classes kept.");
		System.out.println(typeclasseskept + " type classes kept.");

	}


}
