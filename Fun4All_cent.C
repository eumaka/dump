#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <Detinfo.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/CaloWaveformProcessing.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <globalvertex/GlobalVertexReco.h>
#include <zdcinfo/ZdcReco.h>
#include <mbd/MbdReco.h>
#include <epd/EpdReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libDetinfo.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libzdcinfo.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libepd.so)
R__LOAD_LIBRARY(libeventplaneinfo.so)
//R__LOAD_LIBRARY(libGetEPinfo.so)


#endif

void Fun4All_cent(int nevents = 2)
{

    Fun4AllServer *se = Fun4AllServer::instance();
    recoConsts *rc = recoConsts::instance();

    // conditions DB flags and timestamp
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    rc->set_uint64Flag("TIMESTAMP", 54280);

    CDBInterface *cdb = CDBInterface::instance();

    bool prod = false;
    std::string filein = "";
    Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
    
    if(prod)
    {
       filein = "/sphenix/lustre01/sphnxpro/physics/run2auau/caloy2calib/new_2024p007/run_00054200_00054300/DST_CALO_run2auau_new_2024p007-00054280-00000.root";
    }
    else
    {
        filein = "/sphenix/lustre01/sphnxpro/physics/run2auau/calophysics/new_2024p007/run_00054200_00054300/DST_TRIGGERED_EVENT_run2auau_new_2024p007-00054280-00000.root";

        //global--------------------------------------------------
          MbdReco *mbdreco = new MbdReco();
          se->registerSubsystem(mbdreco);
 
/*
         CaloTowerBuilder *ca1 = new CaloTowerBuilder("zdc");
         ca1->set_detector_type(CaloTowerDefs::ZDC);
         ca1->set_nsamples(16);
         ca1->set_builder_type(CaloTowerDefs::kPRDFWaveform);
         ca1->set_processing_type(CaloWaveformProcessing::FAST);
         se->registerSubsystem(ca1);
  */     
        CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;
        CaloTowerBuilder *caZDC = new CaloTowerBuilder("ZDCBUILDER");
        caZDC->set_detector_type(CaloTowerDefs::ZDC);
        caZDC->set_builder_type(buildertype);
        caZDC->set_processing_type(CaloWaveformProcessing::FAST);
        caZDC->set_nsamples(16);
        caZDC->set_offlineflag();
        se->registerSubsystem(caZDC);


          // ZDC Reconstruction--Calib Info
          ZdcReco *zdcreco = new ZdcReco();
          zdcreco->set_zdc1_cut(0.0);
          zdcreco->set_zdc2_cut(0.0);
          se->registerSubsystem(zdcreco);

          GlobalVertexReco *gvertex = new GlobalVertexReco();
          se->registerSubsystem(gvertex);

          CaloTowerBuilder *ca2 = new CaloTowerBuilder("SEPD");
          ca2->set_detector_type(CaloTowerDefs::SEPD);
          ca2->set_processing_type(CaloWaveformProcessing::FAST);
          ca2->set_builder_type(buildertype);
          ca2->set_nsamples(12);
          se->registerSubsystem(ca2);

          //sEPD Reconstruction--Calib Info
          EpdReco *epdreco = new EpdReco();
          se->registerSubsystem(epdreco);
        
          /*
          EventPlaneReco *epreco = new EventPlaneReco();
          epreco->set_sepd_epreco(true);
          se->registerSubsystem(epreco);
          */

        //   filein = "/sphenix/lustre01/sphnxpro/physics/run2auau/caloy2fitting/new_2024p007/run_00054200_00054300/DST_CALOFITTING_run2auau_new_2024p007-00054280-00000.root";

    }




in->AddFile(filein);

se->registerInputManager(in);


MinimumBiasClassifier *mb = new MinimumBiasClassifier();
mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_54280.root");            // Temoporary fix
mb->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_54280.root"); // Temoporary fix
se->registerSubsystem(mb);


CentralityReco *centralityreco = new CentralityReco();
centralityreco->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_54280.root");
centralityreco->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_54280.root");
centralityreco->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_54280.root");
se->registerSubsystem(centralityreco);


gSystem->Load("libDetinfo.so.0.0.0");
Detinfo *detana = new Detinfo();
detana->set_filename("/sphenix/user/eumaka/sPHENIX/xsepdcalibtest.root");
se->registerSubsystem(detana);


se->run(nevents);
se->End();
se->PrintTimer();
gSystem->Exit(0);

return 0;
}
