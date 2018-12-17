//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// Authors: S. Guatelli , M. G. Pia, INFN Genova and F. Ambroglini INFN Perugia, Italy
// 
//

#include <map>

#include "globals.hh"

#include "G4HumanPhantomConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4HumanPhantomSD.hh"
#include "G4SDManager.hh"

#include "G4NistManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
//#include "G4VBodyFactory.hh"
//#include "G4MIRDBodyFactory.hh"
//#include "G4ORNLBodyFactory.hh"

#include "G4PhantomBuilder.hh"
#include "G4FemaleBuilder.hh"
#include "G4MaleBuilder.hh"
#include "G4PhantomHeadBuilder.hh"
#include "G4CustomFemaleBuilder.hh"
#include "G4RunManager.hh"
#include "G4HumanPhantomMaterial.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PVPlacement.hh"

G4HumanPhantomConstruction::G4HumanPhantomConstruction()
{
  messenger = new G4HumanPhantomMessenger(this);
  material = new G4HumanPhantomMaterial();
}

G4HumanPhantomConstruction::~G4HumanPhantomConstruction()
{
  delete material;
  delete messenger;
}

G4VPhysicalVolume* G4HumanPhantomConstruction::Construct()
{
  material -> DefineMaterials();
  
  

  G4BasePhantomBuilder*  builder = 0;

  if (model == "MIRDHead" || model == "ORNLHead")
    { 
      G4cout << "HeadBuilder instantiated" << G4endl;
      builder = new G4PhantomHeadBuilder;
      if (model ==  "MIRDHead") builder->SetModel("MIRD");
      else if (model ==  "ORNLHead") builder->SetModel("ORNLMale");
    }
  else
    {  
      if (sex =="Female") 
	{ 
	  if (model == "MIX") builder = new G4CustomFemaleBuilder;
	  else {builder = new G4FemaleBuilder;}
	  builder->SetModel(model);
	  G4cout <<model << " "<< sex << G4endl;
	}
      else if (sex == "Male") 
	{
	  builder = new G4MaleBuilder;
	  builder->SetModel(model);
          if (model == "MIX") 
	    { 
	      G4cout<< "Custom Male is not available!!! MIRD model is selected !" 
		    << G4endl;
	      model = "MIRD";  
	      builder->SetModel(model);}
	}
    }
  G4VPhysicalVolume* motherVolume = ConstructWorld();
  builder->SetMotherVolume(motherVolume->GetLogicalVolume()->GetDaughter(1));
  
  // the argument indicates the sensitivity of the volume
  
  builder->BuildHead("black", false, sensitivities["Head"]);
  builder->BuildSkull("orange", false,sensitivities["Skull"]); 
  builder->BuildBrain("yellow", true,sensitivities["Brain"]); 

  if (model != "MIRDHead" && model != "ORNLHead")
    { 
      //  builder->SetModel(model);
      builder->BuildTrunk("yellow", false, sensitivities["Trunk"]);
      
      builder->BuildLeftLeg("yellow", false,sensitivities["LeftLeg"]);
      builder->BuildRightLeg("yellow", false,sensitivities["RightLeg"]);
      
      builder->BuildLeftArmBone("grey", true,sensitivities["LeftArmBone"]);
      builder->BuildRightArmBone("grey", true, sensitivities["RightArmBone"]);  
    
      builder->BuildLeftLegBone("grey", true,sensitivities["LeftLegBone"]);
      builder ->BuildRightLegBone("grey", true,sensitivities["RightLegBone"]);
 
      builder->BuildUpperSpine("yellow", true,sensitivities["UpperSpine"]); 
      
      if (model == "MIRD" || model == "MIX") 
	{
	  builder->BuildLeftScapula("grey", true, sensitivities["LeftScapula"]); 
	  builder->BuildRightScapula("grey", true, sensitivities["RightScapula"]);
	  builder->BuildLeftAdrenal("yellow", true, sensitivities["LeftAdrenal"]);
	  builder->BuildRightAdrenal("yellow", true, sensitivities["RightAdrenal"]);
	  builder->BuildThymus("orange", true,sensitivities["Thymus"]); 
	  builder->BuildLeftClavicle("grey", true,sensitivities["LeftClavicle"]);
	  builder->BuildRightClavicle("grey", true,sensitivities["RightClavicle"]);
	  builder->BuildSmallIntestine("orange", true,sensitivities["SmallIntestine"]);
	  builder->BuildRibCage("grey", true,sensitivities["RibCage"]); 
	}
  
      builder->BuildMiddleLowerSpine("yellow", true,sensitivities["MiddleLowerSpine"]);
  
      builder->BuildPelvis("grey", true,sensitivities["Pelvis"]); 
  
      builder->BuildStomach("orange", true,sensitivities["Stomach"]); 
      builder->BuildUpperLargeIntestine("lightBlue", true,sensitivities["UpperLargeIntestine"]);
      builder->BuildLowerLargeIntestine("lightBlue", true,sensitivities["LowerLargeIntestine"]);
         
      builder->BuildSpleen("green", true,sensitivities["Spleen"]);
      builder->BuildPancreas("purple", true,sensitivities["Pancreas"]); 
      builder->BuildLiver("orange", true,sensitivities["Liver"]); 

      builder->BuildLeftKidney("green", true,sensitivities["LeftKidney"]);
      builder->BuildRightKidney("green", true,sensitivities["RightKidney"]);
      builder->BuildUrinaryBladder("green", true,sensitivities["UrinaryBladder"]);
 
      builder->BuildHeart("red", true,sensitivities["Heart"]);// to do MIRD
      builder->BuildLeftLung("blue", true,sensitivities["LeftLung"]);
      builder->BuildRightLung("blue", true,sensitivities["RightLung"]);
      builder->BuildThyroid("orange", true,sensitivities["Thyroid"]); 

      if(sex=="Female"){

	builder->BuildLeftOvary("purple", true,sensitivities["LeftOvary"]);
	builder->BuildRightOvary("purple", true,sensitivities["RightOvary"]);
	builder->BuildUterus("purple", true,sensitivities["Uterus"]);

	if (model == "ORNLFemale" || model == "MIRD")
	  {
	    builder->BuildLeftBreast("purple", true,sensitivities["LeftBreast"]); 
	    builder->BuildRightBreast("purple", true,sensitivities["RightBreast"]);
	  }
	else if (model == "MIX")
	  {
	    builder->BuildVoxelLeftBreast("purple",false, sensitivities["LeftBreast"]); 
	    builder->BuildVoxelRightBreast("purple", false, sensitivities["RightBreast"]);  
	  } 
      }
      
      if(sex=="Male"){
	
	if (model == "MIRD"){ 
	  builder -> BuildMaleGenitalia("yellow",false,sensitivities["MaleGenitalia"]);
	  builder -> BuildLeftTeste("purple",true,sensitivities["LeftTeste"]);
	  builder -> BuildRightTeste("purple",true,sensitivities["RightTeste"]);
	}
	else G4cout <<  "ORNL does not have model for male genitalia and testes yet" << G4endl;
      }
      
    }
//  G4VPhysicalVolume* result=builder->GetPhantom(); 
  delete builder;
  return motherVolume; 
}

void  G4HumanPhantomConstruction::SetBodyPartSensitivity(G4String, G4bool)
{
  G4cout << "This method is not currently working !!!!" << G4endl;
}

G4VPhysicalVolume* G4HumanPhantomConstruction::ConstructWorld()
{
  G4Material* air = material -> GetMaterial("Air");
  //
  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // World Volume
  G4double worldSize = 4. *m ;
  G4Box* world = new G4Box("world", 0.5*worldSize, 0.5*worldSize, 0.5*worldSize);

  G4LogicalVolume* logicWorld = new G4LogicalVolume(world, 
						    air, 
						    "logicalWorld", 0, 0,0);

  G4VPhysicalVolume* motherVolume = new G4PVPlacement(0,G4ThreeVector(),
						      "physicalWorld",
						      logicWorld,
						      0,
						      false,
						      0);

  // Visualization Attributes
  G4VisAttributes* WorldVisAtt = new G4VisAttributes(G4Colour(0.94,0.5,0.5));
    
  WorldVisAtt->SetForceSolid(false);
  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
 
  // Material
  G4Element* elementMg = new G4Element("Magnesium", "Mg",12., 24.3050*g/mole);
  G4Element* elementAl = new G4Element("Aluminium", "Al",13., 26.9815*g/mole);
  G4Element* elementFe = new G4Element("Iron",      "Fe",26.,  55.845*g/mole);
  G4Element* elementSi = new G4Element("Silicon",   "Si",14., 28.0855*g/mole);
  G4Element* elementH  = new G4Element("Hydrogen","H" , 1., 1.01*g/mole);
  G4Element* elementC  = new G4Element("Carbon"  ,"C" , 6., 12.01*g/mole);
  G4Element* elementN  = new G4Element("Nitrogen","N" , 7., 14.01*g/mole);
  G4Element* elementO  = new G4Element("Oxygen"  ,"O" , 8., 16.00*g/mole);
  G4Element* elementCa = new G4Element("Calcium", "Ca", 20., 40.078*g/mole);
  G4Element* elementK = new G4Element("Kalium", "K", 19., 39.0983*g/mole);
  G4Element* elementNa = new G4Element("Natrium", "Na", 11., 22.989*g/mole);

  // Mars Regolith
  G4Material* regolith_mat = new G4Material("Mars_regolith",1.7*g/cm3, 4);
  G4Material* SiO2 = nist -> FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4Material* Fe2O3 = nist -> FindOrBuildMaterial("G4_FERRIC_OXIDE");
  G4Material* H2O = nist-> FindOrBuildMaterial("G4_WATER");
          
  G4Material* Al2CaK2MgNa2O7 = new G4Material("Al2CaK2MgNa2O7", 1.1811*g/cm3, 6);
  Al2CaK2MgNa2O7->AddElement(elementAl, 2);
  Al2CaK2MgNa2O7->AddElement(elementCa,1);
  Al2CaK2MgNa2O7->AddElement(elementK, 2);
  Al2CaK2MgNa2O7->AddElement(elementMg, 1);
  Al2CaK2MgNa2O7->AddElement(elementNa, 2);
  Al2CaK2MgNa2O7->AddElement(elementO, 7);
                          
  regolith_mat->AddMaterial(SiO2, 0.512);
  regolith_mat->AddMaterial(Fe2O3, 0.093);  
  regolith_mat->AddMaterial(Al2CaK2MgNa2O7, 0.321);
  regolith_mat->AddMaterial(H2O, 0.074);

  G4double regolith_depth = 0.999* 2*m;
  G4Box* solidRegolith = new G4Box("Regolith", 0.4999*worldSize, 0.4999*worldSize, 0.5*regolith_depth);
  G4LogicalVolume* logicRegolith = new G4LogicalVolume(solidRegolith, regolith_mat, "Regolith");  
  logicRegolith->SetUserLimits(new G4UserLimits(20*cm));
  new G4PVPlacement(0,G4ThreeVector(0,0,-0.5*regolith_depth), logicRegolith, "Regolith", logicWorld, false,5, true);

  // Mars Atmosphere
  G4double atmosphere_outer_radius = 0.4999*worldSize; // m   
  G4Material* atmosphere_mat = new G4Material("Mars_atmosphere", 0.165*g/cm3,3);
  atmosphere_mat->AddMaterial(nist->FindOrBuildMaterial("G4_CARBON_DIOXIDE"),0.957);
  atmosphere_mat->AddMaterial(nist->FindOrBuildMaterial("G4_lN2"),0.027);
  atmosphere_mat->AddMaterial(nist->FindOrBuildMaterial("G4_Ar"), 0.016);

  G4Sphere* solidAtmosphere = new G4Sphere("Atmosphere", 0, atmosphere_outer_radius, 0, 360*degree, 0, 90*degree);
  G4LogicalVolume* logicAtmosphere = new G4LogicalVolume(solidAtmosphere,atmosphere_mat, "Atmosphere");
  logicAtmosphere->SetUserLimits(new G4UserLimits(10*cm));
  new G4PVPlacement(0, G4ThreeVector(0,0,0.00001*cm), logicAtmosphere,"Atmosphere",logicWorld,false,6, true);

  // Space suit
  G4Material* polycarbonate = nist -> FindOrBuildMaterial("G4_POLYCARBONATE");
  
  G4Box* suit_out = new G4Box("suit_out",20.917*cm, 10.917*cm,87.917*cm);
  G4Box* suit_in = new G4Box("suit_in", 20.5*cm, 10.5*cm, 87.5*cm);
  G4SubtractionSolid* suit_final = 
    new G4SubtractionSolid("suit_final", suit_out, suit_in); 

  G4LogicalVolume* logicSuit = new G4LogicalVolume(suit_final,polycarbonate, "logicalSuit");
  new G4PVPlacement(0, G4ThreeVector(0,0,88.5*cm), logicSuit,"physicalSuit",logicAtmosphere,false,1,true);

  return motherVolume;
}

void G4HumanPhantomConstruction::SetPhantomSex(G4String newSex)
{
  sex=newSex;

  if (sex == "Male")
    {
      G4cout << ">> Male Phantom will be built." << G4endl;
    }
  if (sex == "Female")
    {
      G4cout << ">> Female Phantom will be built." << G4endl;
    }
  if ((sex != "Female") && (sex != "Male"))
    G4cout << sex << " can not be defined!" << G4endl;
}

void G4HumanPhantomConstruction::SetPhantomModel(G4String newModel)
{
  model = newModel;

  if (model == "MIRD")
    {
      G4cout<<" >> Phantom " << model << " will be built."<<G4endl;
    }
  if (model == "ORNLFemale")
    {
      G4cout<<" >> Phantom " << model << " will be built."<<G4endl;
    }

  if (model == "ORNLMale")
    {
      G4cout<<" >> Phantom " << model << " will be built."<<G4endl;
    }

  if (model == "MIX")
    {
      G4cout<<" >> Phantom " << model << " will be built."<<G4endl;
    }
  if (model == "MIRDHead")
    {
      G4cout<<" >> Phantom " << model << " will be built."<<G4endl;
    }

  if (model == "ORNLHead")
    {
      G4cout<<" >> Phantom " << model << " will be built."<<G4endl;
    }
}

void G4HumanPhantomConstruction::ConstructSDandField()
{
   G4HumanPhantomSD* SD = new G4HumanPhantomSD("SD", "HumanPhantomCollection");
   G4SDManager::GetSDMpointer()->AddNewDetector(SD);
if (model != "ORNLMale" && model != "ORNLFemale" && model!= "ORNLHead")  
{
  SetSensitiveDetector("logicalHead",SD);
  SetSensitiveDetector("logicalSkull",SD);
  SetSensitiveDetector("logicalBrain",SD);
  if (model != "MIRDHead")
    { 
      SetSensitiveDetector("logicalTrunk",SD);
      SetSensitiveDetector("logicalLeftLeg",SD); 
      SetSensitiveDetector("logicalRightLeg",SD);
      SetSensitiveDetector("logicalLeftArmBone",SD); 
      SetSensitiveDetector("logicalRightArmBone",SD);
      SetSensitiveDetector("logicalLeftLegBone",SD); 
      SetSensitiveDetector("logicalRightLegBone",SD);
      SetSensitiveDetector("logicalUpperSpine",SD);
      SetSensitiveDetector("logicalLeftScapula",SD);
      SetSensitiveDetector("logicalRightScapula",SD);
      SetSensitiveDetector("logicalLeftAdrenal",SD);
      SetSensitiveDetector("logicalRightAdrenal",SD);      SetSensitiveDetector("logicalThymus",SD);      SetSensitiveDetector("logicalLeftClavicle",SD);
      SetSensitiveDetector("logicalRightClavicle",SD);
      SetSensitiveDetector("logicalSmallIntestine",SD); 
      SetSensitiveDetector("logicalRibCage",SD);       SetSensitiveDetector("logicalMiddleLowerSpine",SD); 
      SetSensitiveDetector("logicalStomach",SD);
      SetSensitiveDetector("logicalUpperLargeIntestine",SD);
      SetSensitiveDetector("logicalLowerLargeIntestine",SD);
      SetSensitiveDetector("logicalSpleen",SD);
      SetSensitiveDetector("logicalPancreas",SD);
      SetSensitiveDetector("logicalLeftKidney",SD);
      SetSensitiveDetector("logicalRightKidney",SD);       
      SetSensitiveDetector("logicalUrinaryBladder",SD);
      SetSensitiveDetector("logicalHeart",SD);
      SetSensitiveDetector("logicalLeftLung",SD);
      SetSensitiveDetector("logicalRightLung",SD);
      SetSensitiveDetector("logicalPelvis",SD);
      SetSensitiveDetector("logicalThyroid",SD);
      SetSensitiveDetector("logicalLiver",SD);

      if(sex=="Female"){

	SetSensitiveDetector("logicalLeftOvary",SD);
        SetSensitiveDetector("logicalRightOvary",SD); 
        SetSensitiveDetector("logicalUterus",SD);
        SetSensitiveDetector("logicalLeftBreast",SD);
        SetSensitiveDetector("logicalRightBreast",SD); 
	}

      if(sex=="Male"){
	
	
	  SetSensitiveDetector("logicalMaleGenitalia",SD);
          SetSensitiveDetector("logicalLeftTeste",SD);
	  SetSensitiveDetector("logicalRightTeste",SD);
	}
	
      }
  }else 
 { 
  SetSensitiveDetector("HeadVolume",SD);
  SetSensitiveDetector("SkullVolume",SD);
  SetSensitiveDetector("BrainVolume",SD);
  G4cout << "Work in progress!!!! " << G4endl;
  G4cout <<"ORNL model!!!! Head is sensitive only!!!" << G4endl;
} 

   
}
