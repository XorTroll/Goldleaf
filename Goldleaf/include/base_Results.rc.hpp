R_DEFINE_NAMESPACE_RESULT_MODULE(goldleaf, 356);

namespace goldleaf {

    R_DEFINE_ERROR_RESULT(NotEnoughSize, 1);
    R_DEFINE_ERROR_RESULT(MetaNotFound, 2);
    R_DEFINE_ERROR_RESULT(CnmtNotFound, 3);
    R_DEFINE_ERROR_RESULT(TitleAlreadyInstalled, 4);
    R_DEFINE_ERROR_RESULT(EntryAlreadyPresent, 5);
    R_DEFINE_ERROR_RESULT(CouldNotLocateTitleContents, 6);
    R_DEFINE_ERROR_RESULT(CouldNotBuildNsp, 7);
    R_DEFINE_ERROR_RESULT(KeyGenerationMismatch, 8);
    R_DEFINE_ERROR_RESULT(InvalidNsp, 9);
    R_DEFINE_ERROR_RESULT(InvalidMeta, 10);
    R_DEFINE_ERROR_RESULT(AssertionFailed, 11);

}